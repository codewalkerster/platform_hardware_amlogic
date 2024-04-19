/*
 * Copyright (c) 2024 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */
#define LOG_TAG "cameraDataSaver"

#include <stdlib.h>
#include <vector>

#include <utils/Mutex.h>


#include "camera_data_saver.h"
#include "CamHalDebugLog.h"


namespace android {

struct DataItem {
    int      id;
    int      size;
    uint8_t *pdata;
};

struct CameraData {
    int id;
    std::vector<struct DataItem> dataItems;
    Mutex  access_mutex;
};

class CameraDataSaverImpl{
public:
    CameraDataSaverImpl();
    ~CameraDataSaverImpl();

    int load(int cam_id, int data_id, uint8_t * data, int bytes);
    int save(int cam_id, int data_id, uint8_t * data, int bytes);

private:
    CameraData camera_data[CAMERA_ID_MAX];
};

//============================================================================================
//
//  camera data save and load
//

static int find_data_item_idx_locked(CameraData *p_camera_data, int data_id)
{
    bool found = false;
    int ii = 0;

    for (ii = 0; ii < p_camera_data->dataItems.size(); ++ii) {
        if (p_camera_data->dataItems[ii].id == data_id) {
            found = true;
            break;
        }
    }

    if (found)
        return ii;

    return -1;
}

static int save_camera_data(CameraData *p_camera_data, int data_id, uint8_t * data, int bytes)
{
    Mutex::Autolock lock(&p_camera_data->access_mutex);
    int data_idx = find_data_item_idx_locked(p_camera_data, data_id);
    if (data_idx >= 0) {
        // found
        if (p_camera_data->dataItems[data_idx].size == bytes && p_camera_data->dataItems[data_idx].pdata) {
            memcpy(p_camera_data->dataItems[data_idx].pdata, data, bytes);
            return 0;
        }
        // found but not match. free space and erase item.
        if (p_camera_data->dataItems[data_idx].pdata)
            free(p_camera_data->dataItems[data_idx].pdata);
        p_camera_data->dataItems.erase(p_camera_data->dataItems.begin() + data_idx);
    }

    // not found
    DataItem data_item;
    data_item.pdata = (uint8_t*) malloc(bytes);
    if (data_item.pdata) {
        memcpy(data_item.pdata, data, bytes);
        data_item.id = data_id;
        data_item.size = bytes;
        p_camera_data->dataItems.emplace_back(data_item);
        return 0;
    } else {
        CAMHAL_LOGE("malloc fail. save camera data fail");
    }
    return -1;
}

static int load_camera_data(CameraData *p_camera_data, int data_id, uint8_t * data, int bytes)
{
    Mutex::Autolock lock(&p_camera_data->access_mutex);
    int data_idx = find_data_item_idx_locked(p_camera_data, data_id);
    if (data_idx >= 0) {
        // found
        if (p_camera_data->dataItems[data_idx].size == bytes && p_camera_data->dataItems[data_idx].pdata) {
            memcpy(data, p_camera_data->dataItems[data_idx].pdata, bytes);
            return 0;
        }
    } else {
        CAMHAL_LOGE("not found. load fail");
    }
    return -1;
}

//============================================================================================
//
//  camera data saver impl
//
static CameraDataSaverImpl  g_data_saver_impl;

static int cam_id_check(int cam_id)
{
    if (cam_id >= CAMERA_ID_MAX || cam_id < 0)
        return -1;

    return 0;
}

static int data_id_check(int data_id)
{
    if (data_id >= DATA_ID_MAX || data_id < 0)
        return -1;

    return 0;
}

static int basic_check(int cam_id, int data_id)
{
    if (cam_id_check(cam_id)) {
        CAMHAL_LOGE("invalid cam id %d", cam_id);
        return -1;
    }

    if (data_id_check(data_id)) {
        CAMHAL_LOGE("invalid data id %d", data_id);
        return -1;
    }
    return 0;
}

CameraDataSaverImpl::CameraDataSaverImpl()
{
    camera_data[0].id = 0;
    camera_data[1].id = 1;
}

CameraDataSaverImpl::~CameraDataSaverImpl()
{
    for (int cam_idx = 0; cam_idx < CAMERA_ID_MAX; ++cam_idx) {
        for (int ii = 0; ii < camera_data[cam_idx].dataItems.size(); ++ii) {
            if (camera_data[cam_idx].dataItems[ii].pdata)
                free(camera_data[cam_idx].dataItems[ii].pdata);
        }
        camera_data[cam_idx].dataItems.clear();
    }
}

int CameraDataSaverImpl::save(int cam_id, int data_id, uint8_t * data, int bytes)
{
    if (basic_check(cam_id, data_id))
        return -1;

    return save_camera_data(&camera_data[cam_id], data_id, data, bytes);
}

int CameraDataSaverImpl::load(int cam_id, int data_id, uint8_t * data, int bytes)
{
    if (basic_check(cam_id, data_id))
        return -1;

    return load_camera_data(&camera_data[cam_id], data_id, data, bytes);
}

//================================================================================
// interface

CameraDataSaver::CameraDataSaver(){}

CameraDataSaver::CameraDataSaver(const CameraDataSaver& other){}

CameraDataSaver* CameraDataSaver::getInstance()
{
    static CameraDataSaver g_CameraDataSaver;
    return &g_CameraDataSaver;
}

int CameraDataSaver::load(int cam_id, int data_id, uint8_t * data, int bytes)
{
    return g_data_saver_impl.load(cam_id, data_id, data, bytes);
}

int CameraDataSaver::save(int cam_id, int data_id, uint8_t * data, int bytes)
{
    return g_data_saver_impl.save(cam_id, data_id, data, bytes);
}

}



/*
 * Copyright (c) 2024 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */


#ifndef CAMHAL_V3_CAMERA_DATA_SAVER_H
#define CAMHAL_V3_CAMERA_DATA_SAVER_H


#include <unistd.h>
#include <stdint.h>


typedef enum cam_id {
    CAMERA_ID_0,
    CAMERA_ID_1,
    CAMERA_ID_MAX
} cam_id_t;

typedef enum camera_data_id{
    SENSOR_AGAIN_HIGH,
    SENSOR_AGAIN_LOW,

    SENSOR_EXPOSURE_LONG,
    SENSOR_EXPOSURE_SHORT,

    ISP_AWB_GAIN,
    ISP_DGAIN,

    ISP_WDR_CFG,
    ISP_LTM_CFG,
    ISP_LTM_ENHC,

    DATA_ID_MAX
} camera_data_id_t;

namespace android {
// in android namespace, for Mutex and AutoLock

class CameraDataSaver{
private:
    CameraDataSaver();
    CameraDataSaver(const CameraDataSaver& other);

public:
    static CameraDataSaver* getInstance();

public:
    int load(int cam_id, int data_id, uint8_t * data, int bytes);
    int save(int cam_id, int data_id, uint8_t * data, int bytes);
};

}

#endif



/*
 * Copyright (c) 2014 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description: c++ file
 */

#define LOG_TAG "HDMIStatus"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <utils/Mutex.h>
#include <sys/ioctl.h>

#include <log/log.h>
#include <cutils/properties.h>
#include <HDMIStatus.h>
#include "EmulatedCameraFactory.h"

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

namespace android {
HDMIStatus* HDMIStatus::mInstance = nullptr;
int HDMIStatus::m_hdmi_fd = -1;
bool HDMIStatus::mIsMipiSensor = false;
struct csiCamConfig* HDMIStatus::mSupportedCfg = nullptr;

struct csiCamConfig ov5640Cfg = {
    .sensorWidth      = 1920,
    .sensorHeight     = 1080,
    .sensorName       = "ov5640",
    .subDevName       = nullptr,
};

char subdevName[][64] = {
    "/dev/v4l-subdev0",
    "/dev/v4l-subdev1",
    "/dev/v4l-subdev2",
    "/dev/v4l-subdev3",
};

struct csiCamConfig *supportedCfgs[] = {
    &ov5640Cfg,
};

HDMIStatus::HDMIStatus()     {
    m_hdmi_fd = open(HDMI_DETECT_PATH, O_RDWR);
    if (m_hdmi_fd < 0 )
        CAMHAL_LOGW("open file(%s) fail: %s", HDMI_DETECT_PATH, strerror(errno));
}

HDMIStatus::~HDMIStatus() {
    if (m_hdmi_fd > 0) {
        close(m_hdmi_fd);
        m_hdmi_fd = -1;
    }
}

HDMIStatus* HDMIStatus::getInstance() {
    CAMHAL_LOGD("%s\n", __FUNCTION__);
    if (mInstance != nullptr)
        return mInstance;
    CAMHAL_LOGD("%s: create new ion object \n", __FUNCTION__);
    mInstance = new HDMIStatus;
    return mInstance;
}

void HDMIStatus::putInstance() {
    CAMHAL_LOGD("%s\n", __FUNCTION__);
    if (mInstance != nullptr) {
        delete mInstance;
        mInstance = nullptr;
    }
}

int HDMIStatus::readHdmiStatus() {
    int status = -1;
    if (m_hdmi_fd > 0) {
        int ret = read(m_hdmi_fd, (void *)(&status), sizeof(int));
        if (ret < 0)
            CAMHAL_LOGE("read failed");
        if (status < 0)
            status = 0;
    }
    return status;
}

int HDMIStatus::getHdmiFd() {
    return m_hdmi_fd;
}


plug_status_e HDMIStatus::getHdmiStatus() {
    return readHdmiStatus() > 0 ? HDMI_PLUG_IN : HDMI_PLUG_OUT;
}

bool HDMIStatus::isStandardMipiCamera() {
    int fd = -1;
    int i, j;
    char readSensorName[64] = {0};
    mIsMipiSensor = false;
    for (i = 0; i < NELEM(subdevName); i++) {
        CAMHAL_LOGD("open dev name %s", subdevName[i]);
        fd = open(subdevName[i], O_RDWR);
        if (fd >= 0) {
            int ret = ioctl(fd, SOC_SENSOR_GET_SENSOR_NAME, readSensorName);
            if (ret < 0) {
                CAMHAL_LOGE("get sensor name fail, errno=%s", strerror(errno));
                close(fd);
            } else {
                close(fd);
                CAMHAL_LOGD("get sensor name %s", readSensorName);
                for (j = 0; j < ARRAY_SIZE(supportedCfgs); j++) {
                    if (strstr(readSensorName, supportedCfgs[j]->sensorName)) {
                        mIsMipiSensor = true;
                        mSupportedCfg = supportedCfgs[j];
                        mSupportedCfg->subDevName = subdevName[i];
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool HDMIStatus::isStandardHDMICamera() {
    if (isStandardMipiCamera()) {
        CAMHAL_LOGD("is mipi camera");
        return true;
    } else {
        char property[PROPERTY_VALUE_MAX];
        property_get("vendor.media.hdmi.vdin.enable", property, "false");
        if (strstr(property, "false"))
            return false;
        return (getHdmiStatus() == HDMI_PLUG_IN);
    }
}

int HDMIStatus::getHdmiPlugStatus(int old_status, int new_status, int port) {
    int plug = -1;
    int port_1 = DETECT_BITS[0];
    int port_2 = DETECT_BITS[1];
    int port_3 = DETECT_BITS[2];
    if (((new_status & port_1) != (old_status & port_1))
        || ((new_status & port_2) != (old_status  & port_2))
        || ((new_status & port_3) != (old_status  & port_3))) {
            if ((new_status & port) != 0) {
                    plug = 1;
            } else {
                    plug = 0;
            }
    }
    return plug;
}


HDMIHotplugThread::HDMIHotplugThread(int _m_hdmi_fd)         :
        Thread(false) {
    epoll_fd = ::epoll_create(30);
    if (epoll_fd > 0) {
        backEvents = new epoll_event[20];
    }
    hdmi_detect_bit = 0;
    m_hdmi_fd = _m_hdmi_fd;
    mRunning = true;
    if (m_hdmi_fd < 0) {
        CAMHAL_LOGW("invalid hdmirx0 fd");
        mRunning = false;
    } else {
        epoll_event event;
        event.data.fd = m_hdmi_fd;
        event.events = EPOLLIN | EPOLLET;
        ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, m_hdmi_fd, &event);
        char property[256];
        property_get("vendor.media.hdmi.vdin.port", property, "1");
        int detect_index = atoi(property) - 1;
        if (detect_index < 0 || detect_index > 2) {
            CAMHAL_LOGE("invalid index set default detect bit rx1");
            hdmi_detect_bit = DETECT_BITS[0];
        } else {
            hdmi_detect_bit = DETECT_BITS[detect_index];
        }
        m_hdmi_status = HDMIStatus::readHdmiStatus();
    }
}

HDMIHotplugThread::~HDMIHotplugThread() {
    if (epoll_fd > 0) {
        close(epoll_fd);
        epoll_fd = -1;
    }
    if (backEvents) {
        delete[] backEvents;
        backEvents = nullptr;
    }
}

void HDMIHotplugThread::requestExit() {
    Mutex::Autolock al(mMutex);
    CAMHAL_LOGV("%s: Requesting thread exit", __FUNCTION__);
    mRunning = false;
}

status_t HDMIHotplugThread::requestExitAndWait() {
    CAMHAL_LOGE("%s: Not implemented. Use requestExit + join instead",
          __FUNCTION__);
    return INVALID_OPERATION;
}


status_t HDMIHotplugThread::readyToRun() {
    return OK;
}
#define port_num 3
bool HDMIHotplugThread::threadLoop() {
    while (mRunning) {
        int num = ::epoll_wait(epoll_fd, backEvents, 20, 250);
        if (num <= 0)
            continue;
        int hdmi_detect_bits = 0;
        for (int i = 0; i < port_num; i++) {
            hdmi_detect_bits += DETECT_BITS[i];
        }
        CAMHAL_LOGE("epoll wait %d fds", num);
        for (int i = 0; i < num; ++i) {
            int fd = backEvents[i].data.fd;
            if (backEvents[i].events & EPOLLIN) {
                if (fd == m_hdmi_fd) {
                    int hdmi_status = HDMIStatus::readHdmiStatus();
                    int plug = HDMIStatus::getHdmiPlugStatus(m_hdmi_status, hdmi_status, hdmi_detect_bits);
                    m_hdmi_status = hdmi_status;
                    if (plug >= 0)
                        gEmulatedCameraFactory.onStatusChanged(HDMI_VDIN_DEV_BEGIN_NUM, plug);
                }
            }
        }
    }
    if (!mRunning)
        return false;
    return true;
}

}

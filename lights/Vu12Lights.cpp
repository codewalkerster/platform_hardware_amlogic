/*
 * Copyright (C) 2024 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Vu12Lights.h"

#include <errno.h>
#include <fcntl.h>
#include <log/log.h>
#include <string>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include <aidl/android/hardware/light/LightType.h>

namespace aidl {
namespace android {
namespace hardware {
namespace light {

int Vu12Lights::access_backlight() {
    std::string check_target_path = "/sys/class/tty/ttyACM";
    std::string target_backlight = "/dev/ttyACM";
    for (int i=0; i<10; i++) {
        auto path = target_backlight + std::to_string(i);

        if (access(path.c_str(), F_OK) < 0)
            break;

        auto check_path = check_target_path + std::to_string(i) + "/device/uevent";

        if (check_vidpid(check_path.c_str()) < 0)
            continue;

        if (check_version(path.c_str()) < 0)
            continue;

        backlight_path = path;
        break;
    }

    if (backlight_path == "")
        return -errno;

    return 0;
}

int Vu12Lights::check_vidpid(const char *path) {
    std::ifstream uevent_path(path);

    if(uevent_path.is_open()) {
        std::string line;
        while (std::getline(uevent_path, line)) {
            if (line.rfind("PRODUCT=", 0) == 0 ) {
                if (line.find ("1a86/fe0c") != std::string::npos)
                    return 0;
                break;
            }
        }
    }
    return -1;
}

int Vu12Lights::check_version(const char* path) {
    return write_int(path, CMD::Version, 0);
}

const char* Vu12Lights::get_path(LightType type) {
    switch (type) {
        case LightType::BACKLIGHT:
            if (backlight_path != "")
                return backlight_path.c_str();
            [[fallthrough]];
        default:
            return "/not_supported";
    }
}

int Vu12Lights::setBacklight(int brightness) {
    std::string bl_path(get_path(LightType::BACKLIGHT));
    return write_int(bl_path.c_str(), CMD::Backlight, brightness);
}

int Vu12Lights::write_int(const char* path, CMD cmd, int value) {
    int fd;

    fd = open(path, O_RDWR);

    char buf[20];
    int count;
    if (fd >= 0) {
        switch (cmd) {
            case CMD::Backlight:
                count =  snprintf(buf, sizeof(buf), "@B%03d#", value);
                break;
            case CMD::Version:
                count =  snprintf(buf, sizeof(buf), "@F%03d#", value);
                break;
            case CMD::Display_Reset:
                count =  snprintf(buf, sizeof(buf), "@R%03d#", value);
                break;
            case CMD::CFG_Init:
                count =  snprintf(buf, sizeof(buf), "@I%03d#", value);
                break;
            default:
                break;
        }
        ssize_t ret = write(fd, buf, (size_t)count);

        char type;
        int data;

        ret = read(fd, buf, sizeof(buf));
        close(fd);

        sscanf(buf, "@%c%d#", &type, &data);

        switch (cmd) {
            case CMD:: Version:
                if (type != 'V') {
                    ret = -1;
                } else {
                    ALOGD("vu12 f/w version - %d\n", data);
                    ret = 0;
                }
                break;
            default:
                ALOGD("result code - %c, value -%d\n", type, data);
                ret = 0;
                break;
        }

        return ret == -1? -errno : 0;
    } else {
        ALOGE("%s() failed to open %s:%s\n", __func__,  path, strerror(errno));
        return -errno;
    }
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl

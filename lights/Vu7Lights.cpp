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

#include "Vu7Lights.h"

#include <errno.h>
#include <fcntl.h>
#include <log/log.h>
#include <string>
#include <unistd.h>

#include <aidl/android/hardware/light/LightType.h>

namespace aidl {
namespace android {
namespace hardware {
namespace light {

bool Vu7Lights::access_backlight() {
    if (access(backlight_path.c_str(), F_OK) == 0)
        return true;
    else
        return false;
}

void Vu7Lights::setBacklight(int brightness) {
    int fd = open(backlight_path.c_str(), O_WRONLY);
    if (fd < 0)
        return;
    sys_write_int(fd, brightness);
    close(fd);
}

int Vu7Lights::sys_write_int(int fd, int value) {
    char buffer[16];
    size_t bytes;
    ssize_t amount;

    bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
    if (bytes >= sizeof(buffer)) return -EINVAL;
    amount = write(fd, buffer, bytes);
    return amount == -1 ? -errno : 0;
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl

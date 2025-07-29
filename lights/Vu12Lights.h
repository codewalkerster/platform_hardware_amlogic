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

#pragma once

#include <aidl/android/hardware/light/LightType.h>

namespace aidl {
namespace android {
namespace hardware {
namespace light {

enum CMD {
    Backlight,
    Version,
    Display_Reset,
    CFG_Init,
};

enum BacklightType {
    NONE,
    PWM,
    VU7,
    VU12
};

class Vu12Lights {
    public:
        int access_backlight();
        const char* get_path(LightType type);
        int setBacklight(int brightness);
    private:
        std::string backlight_path = "";
        int check_vidpid(const char *path);
        int check_version(const char* path);
        int write_int(const char* path, CMD cmd, int value);
};

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl

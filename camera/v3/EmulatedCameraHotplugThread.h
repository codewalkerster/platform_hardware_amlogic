/*
 * Copyright (C) 2013 The Android Open Source Project
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

#ifndef HW_EMULATOR_CAMERA_EMULATED_CAMERA_HOTPLUG_H
#define HW_EMULATOR_CAMERA_EMULATED_CAMERA_HOTPLUG_H

/**
 * This class emulates hotplug events by inotifying on a file, specific
 * to a camera ID. When the file changes between 1/0 the hotplug
 * status goes between PRESENT and NOT_PRESENT.
 *
 * Refer to FAKE_HOTPLUG_FILE in EmulatedCameraHotplugThread.cpp
 */

#include "EmulatedCamera2.h"
#include <utils/String8.h>
#include <utils/Vector.h>
#include <sys/socket.h>
#include <linux/netlink.h>

namespace android {
class EmulatedCameraHotplugThread : public Thread {
  public:
    EmulatedCameraHotplugThread(const int* cameraIdArray, size_t size);
    ~EmulatedCameraHotplugThread();

    virtual void requestExit();
    virtual status_t requestExitAndWait();

  private:


    virtual status_t readyToRun();
    virtual bool threadLoop();

    Mutex mMutex;

    bool mRunning;          // guarding only when it's important
    bool mCheckDevPath;
    int mSocketFd;
    struct sockaddr_nl sa;
};
} // namespace android

#endif

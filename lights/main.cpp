#include "lights.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using ::aidl::android::hardware::light::ILights;
using ::aidl::android::hardware::light::Lights;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    std::shared_ptr<Lights> light = ndk::SharedRefBase::make<Lights>();
    LOG(ERROR) << "--ww--register light hal service-----";
    const std::string instance = std::string() + ILights::descriptor + "/default";
    binder_status_t status = AServiceManager_addService(light->asBinder().get(), instance.c_str());
    if (status != STATUS_OK) {
        LOG(ERROR) << "Could not register" << instance;
        // should abort, but don't want crash loop for local testing
    }

    ABinderProcess_joinThreadPool();

    return 1;  // should not reach
}

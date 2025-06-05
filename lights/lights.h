#pragma once

#include <aidl/android/hardware/light/BnLights.h>
#include <pthread.h>

namespace aidl::android::hardware::light {

class Lights : public BnLights {
    private:
        std::vector<HwLight> availableLights;
        void addLight(LightType const type, int const ordinal);
  public:
        Lights();
        ndk::ScopedAStatus setLightState(int id, const HwLightState& state) override;
        ndk::ScopedAStatus getLights(std::vector<HwLight>* lights) override;
};
}

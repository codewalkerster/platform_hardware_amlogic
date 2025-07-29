#pragma once

#include <aidl/android/hardware/light/BnLights.h>
#include "Vu12Lights.h"
#include "Vu7Lights.h"
#include <pthread.h>

namespace aidl::android::hardware::light {

class Lights : public BnLights {
    private:
        std::vector<HwLight> availableLights;
        BacklightType backlight_type = BacklightType::NONE;
        Vu12Lights vu12lights;
        Vu7Lights vu7lights;
        void addLight(LightType const type, int const ordinal);
  public:
        Lights();
        ndk::ScopedAStatus setLightState(int id, const HwLightState& state) override;
        ndk::ScopedAStatus getLights(std::vector<HwLight>* lights) override;
};
}

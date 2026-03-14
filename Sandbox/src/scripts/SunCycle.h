#pragma once
#include "StrikeEngine.h"
#include <string>

class SunCycle : public Strike::Script {
public:
    void onStart()  override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& e) override;

private:
    float mTimeOfDay   = 10.0f;
    float mDayDuration = 360.0f;
    bool  mPaused      = false;

    Strike::Sun* mSun = nullptr;

    std::string mSkyboxId;
    bool        mSkyboxVisible = true;

    void updateSkybox(float hours);
    void updateFog(float hours);
    float fogDensityForHour(float hours) const;

    static constexpr float kMaxFogDensity = 0.008f;

    void        applySunState();
    float       hoursToAngleDeg(float hours) const;
    glm::uvec3  sunColorForHour(float hours) const;
    float       sunIntensityForHour(float hours) const;
};

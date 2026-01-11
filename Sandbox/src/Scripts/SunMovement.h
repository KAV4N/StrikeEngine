#pragma once
#include "StrikeEngine.h"

class SunMovement : public StrikeEngine::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    StrikeEngine::Sun* mSun = nullptr;
    float mTimeOfDay = 0.0f;

    glm::vec3 getSunColor(float hour);
};

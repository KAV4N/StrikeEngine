#pragma once
#include "StrikeEngine.h"

class CameraMovement : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    float mLastX = 0.0f;
    float mLastY = 0.0f;
    bool  mFirstMouse = true;
    float mSensitivity = 0.1f;
    bool  mRotationEnabled = true;
    bool  mPKeyWasPressed = false;
};
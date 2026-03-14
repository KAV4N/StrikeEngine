#pragma once
#include "StrikeEngine.h"

class PlayerController : public Strike::Script {
public:
    void onStart()                 override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& e) override;

private:
    Strike::Entity mCameraEntity;

    float mLastX       = 0.0f;
    float mLastY       = 0.0f;
    bool  mFirstMouse  = true;
    float mSensitivity = 0.1f;
    float mPitch       = 0.0f;
    float mPitchLimit  = 89.0f;

    float mMoveSpeed   = 60.0f;
    float mSprintMult  = 1.8f;
    float mJumpImpulse = 70.0f;

    float mPlayerHalfH  = 16.0f;
    float mGroundRayExt = 3.0f;
    bool  mIsGrounded   = false;

    bool isGrounded();
};
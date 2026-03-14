#pragma once
#include "StrikeEngine.h"

class PlayerController : public Strike::Script {
public:
    void onStart()                 override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& e) override;

private:
    Strike::Entity mCameraEntity;

    float mSensitivity = 0.1f;
    float mPitch       = 0.0f;
    float mPitchLimit  = 89.0f;

    float mMoveSpeed   = 60.0f;
    float mSprintMult  = 1.8f;
    float mJumpImpulse = 70.0f;

    float mPlayerHalfH  = 16.0f;
    float mGroundRayExt = 3.0f;
    bool  mIsGrounded   = false;

    
    
    
    
    
    static constexpr const char* kFootstepAudioIdA = "footstep_a";
    static constexpr const char* kFootstepAudioIdB = "footstep_b";

    static constexpr float kWalkStepInterval   = 0.45f;
    static constexpr float kSprintStepInterval = 0.28f;

    
    
    Strike::Entity mFootstepEntityA;
    Strike::Entity mFootstepEntityB;

    bool  mNextIsA   = true;
    bool  mWasMoving = false;

    void loadFootstepSounds();
    void updateFootsteps(bool isMoving, bool isSprinting);

    bool isGrounded();
};

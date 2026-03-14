#pragma once
#include "StrikeEngine.h"

class CameraMovement : public Strike::Script
{
public:
    void onStart()  override;
    void onUpdate(float deltaTime) override;

private:
    float mMoveSpeed = 20.0f;
    float mLookSpeed = 90.0f;
};
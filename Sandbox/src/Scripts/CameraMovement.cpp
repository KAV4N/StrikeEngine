#include "CameraMovement.h"

void CameraMovement::onStart()
{
    StrikeEngine::Input::setCursorMode(StrikeEngine::CursorMode::Locked);

    auto [x, y] = StrikeEngine::Input::getMouseXY();
    mLastX = x;
    mLastY = y;
}

void CameraMovement::onUpdate(float deltaTime)
{

    glm::vec3 move(0.0f);
    const float speed = 20.0f * deltaTime;

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_D)) move.x += speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_A)) move.x -= speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_W)) move.z -= speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_S)) move.z += speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_SPACE)) move.y += speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) move.y -= speed;

     if (glm::length(move) > 0.0f) {
        scriptEntity.move(move);
    }


    // Toggle rotation and cursor mode on P press (edge detection)
    bool pPressed = StrikeEngine::Input::isKeyPressed(STRIKE_KEY_P);
    if (pPressed && !mPKeyWasPressed) {
        mRotationEnabled = !mRotationEnabled;

        if (mRotationEnabled) {
            StrikeEngine::Input::setCursorMode(StrikeEngine::CursorMode::Locked);
            mFirstMouse = true; // reset to avoid jump when re-enabling
        } else {
            StrikeEngine::Input::setCursorMode(StrikeEngine::CursorMode::Normal);
        }
    }
    mPKeyWasPressed = pPressed;

    if (!mRotationEnabled)
        return;

    auto [x, y] = StrikeEngine::Input::getMouseXY();

    if (mFirstMouse) {
        mLastX = x;
        mLastY = y;
        mFirstMouse = false;
    }

    float deltaX = x - mLastX;
    float deltaY = y - mLastY;

    mLastX = x;
    mLastY = y;

    glm::vec3 rotation(0.0f);
    rotation.y =  -deltaX * mSensitivity;  // yaw
    rotation.x =  -deltaY * mSensitivity;  // pitch

    if (glm::length(rotation) > 0.0f)
        scriptEntity.rotate(rotation);
}

REGISTER_SCRIPT(CameraMovement);
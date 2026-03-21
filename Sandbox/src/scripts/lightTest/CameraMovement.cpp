#include "CameraMovement.h"

void CameraMovement::onStart()
{
    Strike::Input::setCursorMode(Strike::CursorMode::Normal);
}

void CameraMovement::onUpdate(float deltaTime)
{

    float fps   = Strike::Application::get().getCurrentFPS();
    int   lights = Strike::GameData::get().hasKey("lightCount")
                 ? Strike::GameData::get().getInt("lightCount")
                 : 0;
 
    Strike::Application::get().getWindow().setWindowTitle(
        "StrikeEngine  |  FPS: " + std::to_string(static_cast<int>(fps)) +
        "  |  Lights: "          + std::to_string(lights)
    );

    // Movement
    glm::vec3 move(0.0f);
    float speed = mMoveSpeed * deltaTime;

    if (Strike::Input::isKeyPressed(STRIKE_KEY_W))          move.z -= speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_S))          move.z += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_A))          move.x -= speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_D))          move.x += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_SPACE))      move.y += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) move.y -= speed;

    if (glm::length(move) > 0.0f)
        scriptEntity.move(move);

    // Look
    float look = mLookSpeed * deltaTime;
    glm::vec3 euler(0);

    if (Strike::Input::isKeyPressed(STRIKE_KEY_LEFT))  euler.y += look;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_RIGHT)) euler.y -= look;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_UP))    euler.x += look;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_DOWN))  euler.x -= look;

    scriptEntity.rotate(euler);
}

REGISTER_SCRIPT(CameraMovement)


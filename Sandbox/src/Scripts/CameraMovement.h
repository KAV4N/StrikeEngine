#pragma once
#include "StrikeEngine.h"

class CameraMovement : public StrikeEngine::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onMouseButtonPressed(StrikeEngine::MouseButtonPressedEvent& event) override;

    void swapScene();
    void spawnLight();

    void printStats(const std::string& state);

private:
    StrikeEngine::Entity mCameraEntity;
    int idCounter = 0;
};
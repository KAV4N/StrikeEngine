#pragma once
#include "StrikeEngine.h"

class CameraMovement : public StrikeEngine::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onEvent(StrikeEngine::Event& event) override; // Now void

    bool keyEventTest(StrikeEngine::KeyReleasedEvent& event);

public:
    StrikeEngine::Entity camera;
    int idCounter = 0;
};
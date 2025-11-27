#pragma once
#include "StrikeEngine.h"

class CubeSpawn : public StrikeEngine::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onMouseButtonPressed(StrikeEngine::MouseButtonPressedEvent& event) override;
private:
    void spawnCube();
    int counter = 0;
};
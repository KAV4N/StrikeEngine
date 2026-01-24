#pragma once
#include "StrikeEngine.h"

class CubeSpawn : public StrikeEngine::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
private:
    void spawnCube();
    int counter = 0;
    bool audioPlayed = false;
};
#pragma once
#include "StrikeEngine.h"

class TestScript : public StrikeEngine::Script {
public:
    void onUpdate(float deltaTime) override; // Declaration only
};
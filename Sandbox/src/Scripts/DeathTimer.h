#pragma once
#include "StrikeEngine.h"

class DeathTimer : public StrikeEngine::Script {
public:
    void onUpdate(float deltaTime) override;
};
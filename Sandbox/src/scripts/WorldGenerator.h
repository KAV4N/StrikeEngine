#pragma once
#include "StrikeEngine.h"

class WorldGenerator : public Strike::Script {
public:
    void onStart() override;

private:
    static constexpr int   WORLD_WIDTH  = 32;
    static constexpr int   WORLD_DEPTH  = 32;
    static constexpr int   BASE_HEIGHT  = 3;
    static constexpr int   HEIGHT_RANGE = 4;
    static constexpr float CUBE_SCALE   = 4.0f;

    int   getHeight(int x, int z) const;
    float smoothNoise(float x, float z) const;
    float fade(float t) const;
    float lerp(float a, float b, float t) const;
};
#pragma once
#include "StrikeEngine.h"

#include <random>

// ─────────────────────────────────────────────────────────────────────────────
//  LightSpawner
//
//  Press SPACE to stamp 100 random point lights onto the ground.
//  Movement and camera look are handled by the CameraMovement script
//  on the same entity.
// ─────────────────────────────────────────────────────────────────────────────
class LightBatchSpawner : public Strike::Script
{
public:
    void onStart() override;
    void onEvent(Strike::Event& event) override;

private:
    // ── tuning constants ────────────────────────────────────────────────────
    static constexpr int   LIGHTS_PER_BATCH   = 100;
    static constexpr float GROUND_HALF_EXTENT = 245.0f; // keep within 500×500
    static constexpr float LIGHT_Y            =   4.0f; // height above ground
    static constexpr float LIGHT_RADIUS_MIN   =  20.0f;
    static constexpr float LIGHT_RADIUS_MAX   =  50.0f;
    static constexpr float LIGHT_INTENSITY_MIN =  6.0f;
    static constexpr float LIGHT_INTENSITY_MAX = 20.0f;

    // ── random engine ───────────────────────────────────────────────────────
    std::mt19937 mRng;

    // ── helpers ─────────────────────────────────────────────────────────────
    void spawnLightBatch();
    glm::uvec3 randomLightColor();
};
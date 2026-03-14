#pragma once
#include "StrikeEngine.h"
#include <vector>

class LightSpawner : public Strike::Script {
public:
    void onStart()  override;
    void onEvent(Strike::Event& e) override;

    float mLightIntensity = 10.0f;
    float mLightRadius    = 175.0f;

    static constexpr const char* kAmbientMusicId = "ambient_music";
    float mAmbientVolume = 0.1f;

    static constexpr const char* kLightHumId = "light_hum";
    float mHumVolume  = 10.f;
    float mHumMinDist = 5.0f;
    float mHumMaxDist = 200.0f;

private:
    Strike::Entity mCameraEntity;
    Strike::Entity mAmbientEntity;

    struct SpawnedLight {
        Strike::Entity lightEntity;
        glm::uvec3     color;
    };
    std::vector<SpawnedLight> mLights;

    void startAmbientMusic();
    void spawnLight();
    void removeLastLight();

    static glm::uvec3 randomColor();
};

#include "LightSpawner.h"
#include <cstdlib>
#include <ctime>

void LightSpawner::onStart()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    mCameraEntity = getEntity().getScene()->getEntity("Camera");
    if (!mCameraEntity.isValid())
        STRIKE_CORE_WARN("LightSpawner: could not find 'Camera' - lights will spawn at origin");

    auto& am = Strike::AssetManager::get();

    startAmbientMusic();

    STRIKE_INFO("LightSpawner: ready.  F = spawn light,  G = remove last light");
}

void LightSpawner::startAmbientMusic()
{
    auto scene = getEntity().getScene();

    mAmbientEntity = scene->createEntity();
    mAmbientEntity.setTag("GlobalAmbient");

    if (mCameraEntity.isValid())
        mAmbientEntity.setParent(mCameraEntity);

    auto& amb = mAmbientEntity.addComponent<Strike::AudioSourceComponent>();
    amb.setAudio(kAmbientMusicId);
    amb.setVolume(mAmbientVolume);
    amb.setLoop(true);
    amb.setSpatial(false);
    amb.play();
}

void LightSpawner::onEvent(Strike::Event& e)
{
    if (e.getEventType() != Strike::EventType::KeyPressed) return;

    auto& key = static_cast<Strike::KeyPressedEvent&>(e);
    if (key.getRepeatCount() != 0) return;

    if (key.getKeyCode() == STRIKE_KEY_F) {
        spawnLight();
        e.handled = true;
    } else if (key.getKeyCode() == STRIKE_KEY_G) {
        removeLastLight();
        e.handled = true;
    }
}

void LightSpawner::spawnLight()
{
    glm::vec3 spawnPos(0.0f);
    if (mCameraEntity.isValid())
        spawnPos = mCameraEntity.getWorldPosition();

    auto scene = getEntity().getScene();
    glm::uvec3 color = randomColor();

    auto lightEntity = scene->createEntity();
    lightEntity.setTag("SpawnedLight");
    lightEntity.setWorldPosition(spawnPos);

    auto& light = lightEntity.addComponent<Strike::LightComponent>();
    light.setColor(color);
    light.setIntensity(mLightIntensity);
    light.setRadius(mLightRadius);

    {
        auto& hum = lightEntity.addComponent<Strike::AudioSourceComponent>();
        hum.setAudio(kLightHumId);
        hum.setVolume(mHumVolume);
        hum.setLoop(true);
        hum.setSpatial(true);
        hum.setMinDistance(mHumMinDist);
        hum.setMaxDistance(mHumMaxDist);
        hum.play();
    }

    mLights.push_back({ lightEntity, color });

    STRIKE_INFO("LightSpawner: spawned light #{} - RGB({},{},{}) at ({:.1f},{:.1f},{:.1f})",
                mLights.size(),
                color.r, color.g, color.b,
                spawnPos.x, spawnPos.y, spawnPos.z);
}

void LightSpawner::removeLastLight()
{
    if (mLights.empty()) {
        STRIKE_CORE_WARN("LightSpawner: no spawned lights to remove");
        return;
    }

    auto& last = mLights.back();

    if (last.lightEntity.isValid())
        last.lightEntity.destroy();

    STRIKE_INFO("LightSpawner: removed light #{}", mLights.size());
    mLights.pop_back();
}

glm::uvec3 LightSpawner::randomColor()
{
    unsigned int channels[3] = {
        static_cast<unsigned int>(std::rand() % 256),
        static_cast<unsigned int>(std::rand() % 256),
        static_cast<unsigned int>(std::rand() % 256),
    };

    int brightIdx = std::rand() % 3;
    channels[brightIdx] = 200 + static_cast<unsigned int>(std::rand() % 56);

    return glm::uvec3(channels[0], channels[1], channels[2]);
}

REGISTER_SCRIPT(LightSpawner)

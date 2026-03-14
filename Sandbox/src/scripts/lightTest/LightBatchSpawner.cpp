#include "LightBatchSpawner.h"

void LightBatchSpawner::onStart()
{
    std::random_device rd;
    mRng.seed(rd());

    Strike::GameData::get().setInt("lightCount", 0);

    STRIKE_INFO("LightBatchSpawner ready - press F to spawn {} lights", LIGHTS_PER_BATCH);
}

void LightBatchSpawner::onEvent(Strike::Event& event)
{
    if (event.getEventType() != Strike::EventType::KeyPressed) return;

    auto& keyEvent = static_cast<Strike::KeyPressedEvent&>(event);
    if (keyEvent.getRepeatCount() > 0) return;

    if (keyEvent.getKeyCode() == STRIKE_KEY_F)
    {
        spawnLightBatch();
        event.handled = true;
    }
}

void LightBatchSpawner::spawnLightBatch()
{
    auto* scene = getEntity().getScene();
    if (!scene)
    {
        STRIKE_WARN("LightBatchSpawner: no active scene, cannot spawn lights");
        return;
    }

    std::uniform_real_distribution<float> posDist(-GROUND_HALF_EXTENT, GROUND_HALF_EXTENT);
    std::uniform_real_distribution<float> radiusDist(LIGHT_RADIUS_MIN,    LIGHT_RADIUS_MAX);
    std::uniform_real_distribution<float> intensityDist(LIGHT_INTENSITY_MIN, LIGHT_INTENSITY_MAX);

    for (int i = 0; i < LIGHTS_PER_BATCH; ++i)
    {
        Strike::Entity lightEntity = scene->createEntity();
        lightEntity.setWorldPosition({ posDist(mRng), LIGHT_Y, posDist(mRng) });

        static int sCounter = 0;
        lightEntity.setTag("SpawnedLight_" + std::to_string(sCounter++));

        auto& light = lightEntity.addComponent<Strike::LightComponent>();
        light.setColor(randomLightColor());
        light.setIntensity(intensityDist(mRng));
        light.setRadius(radiusDist(mRng));
    }

    auto& data = Strike::GameData::get();
    int total = data.getInt("lightCount") + LIGHTS_PER_BATCH;
    data.setInt("lightCount", total);

    STRIKE_INFO("LightBatchSpawner: spawned {} lights ({} total)", LIGHTS_PER_BATCH, total);
}

glm::uvec3 LightBatchSpawner::randomLightColor()
{
    static const glm::uvec3 palette[] = {
        { 255, 200,  80 },
        { 255, 140,  40 },
        { 255,  80,  60 },
        {  80, 180, 255 },
        { 120, 255, 180 },
        { 220,  80, 255 },
        { 255, 255, 160 },
        {  60, 220, 255 },
        { 255, 100, 160 },
        { 200, 255, 100 },
    };

    std::uniform_int_distribution<int> idx(0, static_cast<int>(std::size(palette)) - 1);
    return palette[idx(mRng)];
}

REGISTER_SCRIPT(LightBatchSpawner)
#pragma once
#include "StrikeEngine.h"
#include "WorldGenerator.h"

class ChunkManager : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    static constexpr float UPDATE_INTERVAL = 0.10f;
    static constexpr float RADIUS_XZ = 12.0f;
    static constexpr float FORWARD_BLOCKS = 10.0f;
    static constexpr float REAR_BLOCKS = 3.0f;

    Strike::Entity mPlayerEntity;
    WorldGenerator* mWorldGen = nullptr;
    float mBlockStep = 1.0f;

    bool shouldBeActive(const BlockRecord& rec,
                        const glm::vec3& playerPos,
                        const glm::vec3& playerForwardFlat) const;

    void activateBlock(BlockRecord& rec);
    void deactivateBlock(BlockRecord& rec);
};

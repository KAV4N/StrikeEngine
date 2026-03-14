#pragma once
#include "StrikeEngine.h"
#include <vector>

enum class BlockType : uint8_t {
    Stone = 0,
    Dirt,
    Grass
};

struct BlockRecord {
    Strike::Entity entity;
    glm::vec3 worldPos;
    BlockType type;
    bool hasComponents = false;
};

class WorldGenerator : public Strike::Script {
public:
    void onStart() override;

    std::vector<BlockRecord>& getBlocks() { return mBlocks; }
    const glm::vec3& getTargetBlockSize() const { return mTargetBlockSize; }
    glm::vec3 scaleForModel(const char* modelId) const;

    void registerBlock(const Strike::Entity& entity,
                       const glm::vec3& worldPos,
                       BlockType type,
                       bool hasComponents = false)
    {
        mBlocks.push_back({ entity, worldPos, type, hasComponents });
    }

private:
    static constexpr int   WORLD_WIDTH  = 32;
    static constexpr int   WORLD_DEPTH  = 32;
    static constexpr int   BASE_HEIGHT  = 3;
    static constexpr int   HEIGHT_RANGE = 4;
    static constexpr float CUBE_SCALE   = 4.0f;

    glm::vec3                mTargetBlockSize{ 1.0f };
    std::vector<BlockRecord> mBlocks;

    int   getHeight(int x, int z) const;
    float smoothNoise(float x, float z) const;
    float fade(float t) const;
    float lerp(float a, float b, float t) const;
};

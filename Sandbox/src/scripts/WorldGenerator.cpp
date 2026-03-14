#include "WorldGenerator.h"
#include <cmath>

float WorldGenerator::fade(float t) const {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float WorldGenerator::lerp(float a, float b, float t) const {
    return a + t * (b - a);
}

static float grad(int ix, int iz) {
    int h = ix * 1619 + iz * 31337;
    h = (h ^ (h >> 13)) * 1274126177;
    return static_cast<float>(h & 0xFFFF) / 32767.5f - 1.0f;
}

float WorldGenerator::smoothNoise(float x, float z) const {
    int x0 = static_cast<int>(std::floor(x));
    int z0 = static_cast<int>(std::floor(z));
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    float fx = x - static_cast<float>(x0);
    float fz = z - static_cast<float>(z0);

    float u = fade(fx);
    float v = fade(fz);

    float n00 = grad(x0, z0);
    float n10 = grad(x1, z0);
    float n01 = grad(x0, z1);
    float n11 = grad(x1, z1);

    return lerp(lerp(n00, n10, u), lerp(n01, n11, u), v);
}

int WorldGenerator::getHeight(int x, int z) const {
    float nx = static_cast<float>(x) * 0.18f;
    float nz = static_cast<float>(z) * 0.18f;

    float n = smoothNoise(nx, nz) * 0.65f
            + smoothNoise(nx * 2.1f, nz * 2.1f) * 0.35f;

    float t = (n + 1.0f) * 0.5f;
    return BASE_HEIGHT + static_cast<int>(std::round(t * HEIGHT_RANGE));
}

glm::vec3 WorldGenerator::scaleForModel(const char* modelId) const
{
    auto& am = Strike::AssetManager::get();
    auto  model = am.getAsset<Strike::Model>(modelId);
    if (!model || !model->isReady()) {
        STRIKE_WARN("WorldGenerator: model '{}' not ready, falling back to target size", modelId);
        return mTargetBlockSize;
    }
    glm::vec3 meshSize = model->getBounds().getSize();
    return glm::vec3(
        mTargetBlockSize.x / meshSize.x,
        mTargetBlockSize.y / meshSize.y,
        mTargetBlockSize.z / meshSize.z
    );
}

void WorldGenerator::onStart()
{
    auto& am = Strike::AssetManager::get();

    auto refModel = am.getAsset<Strike::Model>("dirt");
    if (!refModel || !refModel->isReady()) {
        STRIKE_ERROR("WorldGenerator: 'dirt' model not ready - cannot determine block size");
        return;
    }

    mTargetBlockSize = refModel->getBounds().getSize() * CUBE_SCALE;

    Strike::GameData::get().setFloat("blockStep",  mTargetBlockSize.y);
    Strike::GameData::get().setFloat("blockSizeX", mTargetBlockSize.x);
    Strike::GameData::get().setFloat("blockSizeY", mTargetBlockSize.y);
    Strike::GameData::get().setFloat("blockSizeZ", mTargetBlockSize.z);

    float stepX = mTargetBlockSize.x;
    float stepY = mTargetBlockSize.y;
    float stepZ = mTargetBlockSize.z;

    float offsetX = -(WORLD_WIDTH  * stepX) * 0.5f;
    float offsetZ = -(WORLD_DEPTH  * stepZ) * 0.5f;

    auto scene = getEntity().getScene();

    int blockCount   = 0;
    int maxColHeight = 0;

    mBlocks.reserve(WORLD_WIDTH * WORLD_DEPTH * (BASE_HEIGHT + HEIGHT_RANGE));

    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int z = 0; z < WORLD_DEPTH; ++z) {

            int columnHeight = getHeight(x, z);
            if (columnHeight > maxColHeight) maxColHeight = columnHeight;

            for (int y = 0; y < columnHeight; ++y) {

                glm::vec3 pos(
                    offsetX + x * stepX + stepX * 0.5f,
                    y * stepY + stepY * 0.5f,
                    offsetZ + z * stepZ + stepZ * 0.5f
                );

                BlockType type;
                if (y == columnHeight - 1)
                    type = BlockType::Grass;
                else if (y >= columnHeight - 2)
                    type = BlockType::Dirt;
                else
                    type = BlockType::Stone;

                const char* mid = (type == BlockType::Grass) ? "grass"
                                : (type == BlockType::Dirt)  ? "dirt"
                                :                              "stone";

                Strike::Entity block = scene->createEntity();
                block.setTag("Block");
                block.setWorldPosition(pos);
                block.setScale(scaleForModel(mid));

                mBlocks.push_back({ block, pos, type, false });
                ++blockCount;
            }
        }
    }

    STRIKE_INFO("WorldGenerator: registered {} bare block entities ({}x{}, max height {})",
                blockCount, WORLD_WIDTH, WORLD_DEPTH, BASE_HEIGHT + HEIGHT_RANGE);
}

REGISTER_SCRIPT(WorldGenerator)

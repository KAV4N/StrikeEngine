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
    int height = BASE_HEIGHT + static_cast<int>(std::round(t * HEIGHT_RANGE));
    return height;
}

void WorldGenerator::onStart()
{
    auto& am = Strike::AssetManager::get();

    auto cubeModel = am.getAsset<Strike::Model>("dirt");

    if (!cubeModel || !cubeModel->isReady()) {
        STRIKE_ERROR("WorldGenerator: failed to load cube model!");
        return;
    }

    const Strike::Bounds& bounds = cubeModel->getBounds();
    glm::vec3 cubeSize = bounds.getSize();

    STRIKE_INFO("WorldGenerator: cube bounds size = ({:.3f}, {:.3f}, {:.3f})",
                cubeSize.x, cubeSize.y, cubeSize.z);

    float stepX = cubeSize.x * CUBE_SCALE;
    float stepY = cubeSize.y * CUBE_SCALE;
    float stepZ = cubeSize.z * CUBE_SCALE;

    Strike::GameData::get().setFloat("blockStep",  stepY);
    Strike::GameData::get().setFloat("cubeScale",  CUBE_SCALE);

    float offsetX = -(WORLD_WIDTH * stepX) * 0.5f;
    float offsetZ = -(WORLD_DEPTH * stepZ) * 0.5f;

    auto scene = getEntity().getScene();
    int blockCount   = 0;
    int maxColHeight = 0;

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

                Strike::Entity block = scene->createEntity();
                block.setTag("Block");
                block.setWorldPosition(pos);
                block.setScale(glm::vec3(CUBE_SCALE));

                auto& renderer = block.addComponent<Strike::RendererComponent>();
                
                renderer.setTexture("block_texture");

                if (y == columnHeight - 1) {
                    renderer.setModel("grass");                    
                } else if (y >= columnHeight - 3) {
                    renderer.setModel("dirt");
                } else {
                    renderer.setModel("stone");
                }

                auto& physics = block.addComponent<Strike::PhysicsComponent>();
                physics.setAnchored(true);
                physics.setCanCollide(true);

                ++blockCount;
            }
        }
    }

    STRIKE_INFO("WorldGenerator: spawned {} blocks ({}x{}, max height {})",
                blockCount, WORLD_WIDTH, WORLD_DEPTH, BASE_HEIGHT + HEIGHT_RANGE);
}

REGISTER_SCRIPT(WorldGenerator)
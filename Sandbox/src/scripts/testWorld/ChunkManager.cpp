#include "ChunkManager.h"

static const char* modelId(BlockType t) {
    switch (t) {
        case BlockType::Grass: return "grass";
        case BlockType::Dirt:  return "dirt";
        case BlockType::Stone:
        default:               return "stone";
    }
}

void ChunkManager::onStart()
{
    auto wgEntity = getEntity().getScene()->getEntity("WorldGenerator");
    if (!wgEntity.isValid()) {
        STRIKE_ERROR("ChunkManager: could not find entity tagged 'WorldGenerator'");
        return;
    }

    if (!wgEntity.hasComponent<Strike::LogicComponent>()) {
        STRIKE_ERROR("ChunkManager: 'WorldGenerator' entity has no LogicComponent");
        return;
    }

    mWorldGen = wgEntity.getComponent<Strike::LogicComponent>()
                        .getScript<WorldGenerator>();

    if (!mWorldGen) {
        STRIKE_ERROR("ChunkManager: WorldGenerator script not found on entity");
        return;
    }

    mPlayerEntity = getEntity().getScene()->getEntity("Player");
    if (!mPlayerEntity.isValid())
        STRIKE_CORE_WARN("ChunkManager: could not find entity tagged 'Player' - using origin");

    auto& gd = Strike::GameData::get();
    if (gd.hasKey("blockStep"))
        mBlockStep = gd.getFloat("blockStep");
    else
        STRIKE_CORE_WARN("ChunkManager: 'blockStep' not in GameData, defaulting to 1.0");
}

bool ChunkManager::shouldBeActive(const BlockRecord& rec,
                                  const glm::vec3& playerPos,
                                  const glm::vec3& playerForwardFlat) const
{
    glm::vec2 delta = glm::vec2(rec.worldPos.x - playerPos.x,
                                rec.worldPos.z - playerPos.z)
                      / mBlockStep;

    float xzDist = glm::length(delta);
    if (xzDist > RADIUS_XZ) return false;

    glm::vec2 fwd2 = glm::vec2(playerForwardFlat.x, playerForwardFlat.z);
    float dot = glm::dot(delta, fwd2);

    if (dot >= 0.0f) {
        return dot <= FORWARD_BLOCKS;
    } else {
        return (-dot) <= REAR_BLOCKS;
    }
}

void ChunkManager::activateBlock(BlockRecord& rec)
{
    if (rec.hasComponents) return;
    if (!rec.entity.isValid()) return;

    auto& renderer = rec.entity.addComponent<Strike::RendererComponent>();
    renderer.setModel(modelId(rec.type));
    renderer.setTexture("block_texture");

    auto& physics = rec.entity.addComponent<Strike::PhysicsComponent>();
    physics.setAnchored(true);
    physics.setCanCollide(true);

    rec.hasComponents = true;
}

void ChunkManager::deactivateBlock(BlockRecord& rec)
{
    if (!rec.hasComponents) return;
    if (!rec.entity.isValid()) return;

    if (rec.entity.hasComponent<Strike::PhysicsComponent>())
        rec.entity.removeComponent<Strike::PhysicsComponent>();

    if (rec.entity.hasComponent<Strike::RendererComponent>())
        rec.entity.removeComponent<Strike::RendererComponent>();

    rec.hasComponents = false;
}

void ChunkManager::onUpdate(float deltaTime)
{
    if (!tick(UPDATE_INTERVAL)) return;
    if (!mWorldGen) return;

    glm::vec3 playerPos(0.0f);
    glm::vec3 playerForwardFlat(0.0f, 0.0f, -1.0f);

    if (mPlayerEntity.isValid()) {
        playerPos = mPlayerEntity.getWorldPosition();

        glm::vec3 fwd = mPlayerEntity.getForward();
        fwd.y = 0.0f;
        if (glm::length(fwd) > 0.001f)
            playerForwardFlat = glm::normalize(fwd);
    }

    auto& blocks = mWorldGen->getBlocks();
    for (auto& rec : blocks) {
        bool wanted = shouldBeActive(rec, playerPos, playerForwardFlat);

        if (wanted && !rec.hasComponents)
            activateBlock(rec);
        else if (!wanted && rec.hasComponents)
            deactivateBlock(rec);
    }
}

REGISTER_SCRIPT(ChunkManager)

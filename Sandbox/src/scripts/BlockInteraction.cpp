#include "BlockInteraction.h"

void BlockInteraction::onStart()
{
    mCameraEntity = getEntity().getScene()->getEntity("Camera");

    if (!mCameraEntity.isValid())
        STRIKE_WARN("BlockInteraction: could not find child entity tagged 'Camera'");

    auto& gd = Strike::GameData::get();
    if (gd.hasKey("cubeScale"))
        mCubeScale = gd.getFloat("cubeScale");
    else
        STRIKE_WARN("BlockInteraction: 'cubeScale' not found in GameData, defaulting to 0.8");
}

void BlockInteraction::clearHover()
{
    if (!mHoveredBlock.isValid()) return;

    if (mHoveredBlock.hasComponent<Strike::RendererComponent>())
        mHoveredBlock.getComponent<Strike::RendererComponent>()
            .setColor(mHoveredOriginalColor);

    mHoveredBlock = Strike::Entity{};
}

void BlockInteraction::updateHover()
{
    if (!mCameraEntity.isValid()) return;

    glm::vec3 origin = mCameraEntity.getWorldPosition();
    glm::vec3 dir    = mCameraEntity.getForward();

    auto hit = Strike::World::get().rayCast(origin, dir, mReach);

    if (!hit.hasHit() || hit.entity.getTag() != "Block")
    {
        clearHover();
        return;
    }

    if (hit.entity == mHoveredBlock) return;

    clearHover();

    if (!hit.entity.hasComponent<Strike::RendererComponent>()) return;

    auto& renderer           = hit.entity.getComponent<Strike::RendererComponent>();
    mHoveredOriginalColor    = renderer.getColor();
    mHoveredBlock            = hit.entity;

    glm::uvec3 dark = glm::uvec3(
        static_cast<unsigned int>(mHoveredOriginalColor.r * 0.55f),
        static_cast<unsigned int>(mHoveredOriginalColor.g * 0.55f),
        static_cast<unsigned int>(mHoveredOriginalColor.b * 0.55f)
    );
    renderer.setColor(dark);
}

void BlockInteraction::onUpdate(float deltaTime)
{
    updateHover();
}

glm::vec3 BlockInteraction::dominantNormal(const glm::vec3& normal) const
{
    glm::vec3 absN = glm::abs(normal);

    if (absN.x >= absN.y && absN.x >= absN.z)
        return glm::vec3(normal.x > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);
    else if (absN.y >= absN.x && absN.y >= absN.z)
        return glm::vec3(0.0f, normal.y > 0.0f ? 1.0f : -1.0f, 0.0f);
    else
        return glm::vec3(0.0f, 0.0f, normal.z > 0.0f ? 1.0f : -1.0f);
}

void BlockInteraction::breakBlock()
{
    if (!mCameraEntity.isValid()) return;

    glm::vec3 origin = mCameraEntity.getWorldPosition();
    glm::vec3 dir    = mCameraEntity.getForward();

    auto hit = Strike::World::get().rayCast(origin, dir, mReach);
    if (!hit.hasHit()) return;
    if (hit.entity == getEntity()) return;
    if (hit.entity.getTag() != "Block") return;

    if (hit.entity == mHoveredBlock)
        mHoveredBlock = Strike::Entity{};

    hit.entity.destroy();
}

void BlockInteraction::placeBlock()
{
    if (!mCameraEntity.isValid()) return;

    glm::vec3 origin = mCameraEntity.getWorldPosition();
    glm::vec3 dir    = mCameraEntity.getForward();

    auto hit = Strike::World::get().rayCast(origin, dir, mReach);
    if (!hit.hasHit()) return;
    if (hit.entity.getTag() != "Block") return;

    glm::vec3 hitBlockCenter = hit.entity.getWorldPosition();
    glm::vec3 hitBlockScale  = hit.entity.getWorldScale();

    auto& am    = Strike::AssetManager::get();
    auto  model = am.getAsset<Strike::Model>("stone");
    if (!model || !model->isReady()) {
        STRIKE_WARN("BlockInteraction: cube model not ready");
        return;
    }

    glm::vec3 rawSize        = model->getBounds().getSize();
    glm::vec3 blockWorldSize = rawSize * hitBlockScale;
    glm::vec3 faceNormal     = dominantNormal(hit.normal);
    glm::vec3 placePos       = hitBlockCenter + faceNormal * blockWorldSize;

    if (glm::length(placePos - scriptEntity.getWorldPosition()) < blockWorldSize.y * 1.1f) return;

    auto scene = getEntity().getScene();
    auto block = scene->createEntity();
    block.setTag("Block");
    block.setWorldPosition(placePos);
    block.setScale(glm::vec3(mCubeScale));

    auto& renderer = block.addComponent<Strike::RendererComponent>();
    renderer.setModel("stone");
    renderer.setTexture("block_texture");

    auto& physics = block.addComponent<Strike::PhysicsComponent>();
    physics.setAnchored(true);
    physics.setCanCollide(true);
}

void BlockInteraction::onEvent(Strike::Event& e)
{
    if (e.getEventType() != Strike::EventType::MouseButtonPressed) return;

    auto& mouse = static_cast<Strike::MouseButtonPressedEvent&>(e);

    if (mouse.getMouseButton() == STRIKE_MOUSE_BUTTON_LEFT) {
        breakBlock();
        e.handled = true;
    } else if (mouse.getMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
        placeBlock();
        e.handled = true;
    }
}

REGISTER_SCRIPT(BlockInteraction)
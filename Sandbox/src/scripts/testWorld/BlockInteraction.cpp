#include "BlockInteraction.h"
#include <algorithm>

constexpr BlockInteraction::BlockDef BlockInteraction::kBlockDefs[BlockInteraction::kBlockTypeCount];
constexpr glm::vec3 BlockInteraction::kHandBasePos;
constexpr glm::vec3 BlockInteraction::kHandBasePeak;
constexpr glm::vec3 BlockInteraction::kHandBaseRot;
constexpr glm::vec3 BlockInteraction::kHandPeakRot;

static float easeOutCubic(float t) { return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }
static float easeInCubic (float t) { return t * t * t; }

void BlockInteraction::notifyBlockDestroyed(const Strike::Entity& entity)
{
    auto wgEntity = getEntity().getScene()->getEntity("WorldGenerator");
    if (!wgEntity.isValid()) return;
    if (!wgEntity.hasComponent<Strike::LogicComponent>()) return;

    auto* wg = wgEntity.getComponent<Strike::LogicComponent>()
                       .getScript<WorldGenerator>();
    if (!wg) return;

    for (auto& rec : wg->getBlocks()) {
        if (rec.entity == entity) {
            rec.hasComponents = false;
            break;
        }
    }
}

void BlockInteraction::loadBlockSounds()
{
    auto& am = Strike::AssetManager::get();

    auto scene = getEntity().getScene();

    
    mPlaceSoundEntity = scene->createEntity();
    mPlaceSoundEntity.setTag("BlockPlaceSound");
    if (mCameraEntity.isValid())
        mPlaceSoundEntity.setParent(mCameraEntity);

    auto& placeSrc = mPlaceSoundEntity.addComponent<Strike::AudioSourceComponent>();
    placeSrc.setAudio(kPlaceSoundId);
    placeSrc.setVolume(0.8f);
    placeSrc.setLoop(false);
    placeSrc.setSpatial(false);

    
    mBreakSoundEntity = scene->createEntity();
    mBreakSoundEntity.setTag("BlockBreakSound");
    if (mCameraEntity.isValid())
        mBreakSoundEntity.setParent(mCameraEntity);

    auto& breakSrc = mBreakSoundEntity.addComponent<Strike::AudioSourceComponent>();
    breakSrc.setAudio(kBreakSoundId);
    breakSrc.setVolume(0.9f);
    breakSrc.setLoop(false);
    breakSrc.setSpatial(false);
}

void BlockInteraction::playBlockSound(Strike::Entity& soundEntity)
{
    if (!soundEntity.isValid()) return;
    if (!soundEntity.hasComponent<Strike::AudioSourceComponent>()) return;
    soundEntity.getComponent<Strike::AudioSourceComponent>().play();
}

void BlockInteraction::onStart()
{
    mCameraEntity = getEntity().getScene()->getEntity("Camera");
    if (!mCameraEntity.isValid())
        STRIKE_WARN("BlockInteraction: could not find 'Camera'");

    if (mCameraEntity.isValid()) {
        auto scene  = getEntity().getScene();
        mHandEntity = scene->createEntity();
        mHandEntity.setTag("HandBlock");
        mHandEntity.setParent(mCameraEntity);
        mHandEntity.setPosition(kHandBasePos);
        mHandEntity.setEulerAngles(kHandBaseRot);
        mHandEntity.setScale(glm::vec3(kHandBaseScale));

        auto& renderer = mHandEntity.addComponent<Strike::RendererComponent>();
        renderer.setModel(selectedModelId());
        renderer.setTexture("block_texture");
    }

    loadBlockSounds();
}

void BlockInteraction::cycleBlockType(int delta)
{
    mSelectedIndex = (mSelectedIndex + delta + kBlockTypeCount) % kBlockTypeCount;
    updateHandVisual();
}

void BlockInteraction::updateHandVisual()
{
    if (!mHandEntity.isValid()) return;
    if (!mHandEntity.hasComponent<Strike::RendererComponent>()) return;
    mHandEntity.getComponent<Strike::RendererComponent>().setModel(selectedModelId());
}

void BlockInteraction::triggerSwing()
{
    mSwinging      = true;
    mSwingProgress = 0.0f;
}

void BlockInteraction::updateSwing()
{
    if (!mSwinging || !mHandEntity.isValid()) return;

    if (tick(kSwingDuration / 2.0f)) {
        if (mSwingProgress < kSwingMid) {
            mSwingProgress = kSwingMid;
        } else {
            mSwingProgress = 1.0f;
            mSwinging = false;
        }
    }

    glm::vec3 pos;
    glm::vec3 rot;

    if (mSwingProgress < kSwingMid) {
        float t = easeInCubic(mSwingProgress / kSwingMid);
        pos = glm::mix(kHandBasePos,  kHandBasePeak, t);
        rot = glm::mix(kHandBaseRot,  kHandPeakRot,  t);
    } else {
        float t = easeOutCubic((mSwingProgress - kSwingMid) / (1.0f - kSwingMid));
        pos = glm::mix(kHandBasePeak, kHandBasePos,  t);
        rot = glm::mix(kHandPeakRot,  kHandBaseRot,  t);
    }

    mHandEntity.setPosition(pos);
    mHandEntity.setEulerAngles(rot);
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

    if (!hit.hasHit() || hit.entity.getTag() != "Block") {
        clearHover();
        return;
    }

    if (hit.entity == mHoveredBlock) return;

    clearHover();
    if (!hit.entity.hasComponent<Strike::RendererComponent>()) return;

    auto& renderer        = hit.entity.getComponent<Strike::RendererComponent>();
    mHoveredOriginalColor = renderer.getColor();
    mHoveredBlock         = hit.entity;

    renderer.setColor(glm::uvec3(
        static_cast<unsigned int>(mHoveredOriginalColor.r * 0.55f),
        static_cast<unsigned int>(mHoveredOriginalColor.g * 0.55f),
        static_cast<unsigned int>(mHoveredOriginalColor.b * 0.55f)
    ));
}

void BlockInteraction::updateThrownBlocks(float deltaTime)
{
    for (auto& tb : mThrownBlocks)
        tb.timer += deltaTime;

    mThrownBlocks.erase(
        std::remove_if(mThrownBlocks.begin(), mThrownBlocks.end(),
            [](ThrownBlock& tb) {
                if (tb.timer >= kThrownBlockLifetime) {
                    if (tb.entity.isValid())
                        tb.entity.destroy();
                    return true;
                }
                return false;
            }),
        mThrownBlocks.end());
}

void BlockInteraction::onUpdate(float deltaTime)
{
    updateHover();
    updateSwing();
    updateThrownBlocks(deltaTime);
}

glm::vec3 BlockInteraction::dominantNormal(const glm::vec3& normal) const
{
    glm::vec3 absN = glm::abs(normal);

    if (absN.x >= absN.y && absN.x >= absN.z)
        return { normal.x > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f };
    else if (absN.y >= absN.x && absN.y >= absN.z)
        return { 0.0f, normal.y > 0.0f ? 1.0f : -1.0f, 0.0f };
    else
        return { 0.0f, 0.0f, normal.z > 0.0f ? 1.0f : -1.0f };
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

    triggerSwing();
    playBlockSound(mBreakSoundEntity);
    notifyBlockDestroyed(hit.entity);
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

    auto wgEntity = getEntity().getScene()->getEntity("WorldGenerator");
    WorldGenerator* wg = nullptr;
    if (wgEntity.isValid() && wgEntity.hasComponent<Strike::LogicComponent>())
        wg = wgEntity.getComponent<Strike::LogicComponent>().getScript<WorldGenerator>();

    if (!wg) {
        STRIKE_WARN("BlockInteraction: WorldGenerator not found, cannot place block");
        return;
    }

    glm::vec3 hitBlockCenter  = hit.entity.getWorldPosition();
    glm::vec3 hitBlockScale   = hit.entity.getWorldScale();

    auto& am    = Strike::AssetManager::get();
    auto  model = am.getAsset<Strike::Model>(selectedModelId());
    if (!model || !model->isReady()) {
        STRIKE_WARN("BlockInteraction: model '{}' not ready", selectedModelId());
        return;
    }

    glm::vec3 targetSize     = wg->getTargetBlockSize();
    glm::vec3 faceNormal     = dominantNormal(hit.normal);
    glm::vec3 placePos       = hitBlockCenter + faceNormal * targetSize;

    if (glm::length(placePos - getEntity().getWorldPosition()) < targetSize.y * 1.1f)
        return;

    triggerSwing();
    playBlockSound(mPlaceSoundEntity);

    auto scene = getEntity().getScene();
    auto block = scene->createEntity();
    block.setTag("Block");
    block.setWorldPosition(placePos);
    block.setScale(wg->scaleForModel(selectedModelId()));

    auto& renderer = block.addComponent<Strike::RendererComponent>();
    renderer.setModel(selectedModelId());
    renderer.setTexture("block_texture");

    auto& physics = block.addComponent<Strike::PhysicsComponent>();
    physics.setAnchored(true);
    physics.setCanCollide(true);

    wg->registerBlock(block, placePos, selectedBlockType(), true);
}

void BlockInteraction::throwBlock()
{
    if (!mCameraEntity.isValid()) return;

    glm::vec3 camPos     = mCameraEntity.getWorldPosition();
    glm::vec3 camForward = mCameraEntity.getForward();
    glm::vec3 camRight   = mCameraEntity.getRight();
    glm::vec3 camUp      = mCameraEntity.getUp();

    auto wgEntity = getEntity().getScene()->getEntity("WorldGenerator");
    WorldGenerator* wg = nullptr;
    if (wgEntity.isValid() && wgEntity.hasComponent<Strike::LogicComponent>())
        wg = wgEntity.getComponent<Strike::LogicComponent>().getScript<WorldGenerator>();

    glm::vec3 blockScale = wg ? wg->scaleForModel(selectedModelId()) : glm::vec3(1.0f);
    float     spawnOffset = wg ? wg->getTargetBlockSize().z * 1.5f : 6.0f;

    glm::vec3 spawnPos = camPos + camForward * spawnOffset;

    auto  scene = getEntity().getScene();
    auto  block = scene->createEntity();
    block.setTag("ThrownBlock");
    block.setWorldPosition(spawnPos);
    block.setScale(blockScale);

    auto& renderer = block.addComponent<Strike::RendererComponent>();
    renderer.setModel(selectedModelId());
    renderer.setTexture("block_texture");

    auto& physics = block.addComponent<Strike::PhysicsComponent>();
    physics.setAnchored(false);
    physics.setCanCollide(true);
    physics.setMass(mThrowMass);
    physics.setFriction(0.5f);
    physics.setBounciness(0.25f);
    physics.setLinearDamping(0.05f);
    physics.setAngularDamping(0.03f);

    glm::vec3 throwDir = glm::normalize(camForward + camUp * mThrowArcBias);
    physics.setVelocity(throwDir * mThrowSpeed);

    glm::vec3 angVel = camForward * mThrowAngularVel.z
                     + camRight   * mThrowAngularVel.x
                     + camUp      * mThrowAngularVel.y;
    physics.setAngularVelocity(angVel);

    triggerSwing();
    playBlockSound(mPlaceSoundEntity);

    mThrownBlocks.push_back({ block, 0.0f });

    STRIKE_INFO("BlockInteraction: threw '{}' - speed={:.0f}  mass={:.1f}",
                selectedModelId(), mThrowSpeed, mThrowMass);
}

void BlockInteraction::onEvent(Strike::Event& e)
{
    if (e.getEventType() == Strike::EventType::MouseScrolled) {
        auto& scroll = static_cast<Strike::MouseScrolledEvent&>(e);
        cycleBlockType(scroll.getYOffset() > 0.0f ? -1 : 1);
        e.handled = true;
        return;
    }

    if (e.getEventType() == Strike::EventType::KeyPressed) {
        auto& key = static_cast<Strike::KeyPressedEvent&>(e);
        if (key.getRepeatCount() == 0) {
            if (key.getKeyCode() == STRIKE_KEY_1) { mSelectedIndex = 0; updateHandVisual(); e.handled = true; return; }
            if (key.getKeyCode() == STRIKE_KEY_2) { mSelectedIndex = 1; updateHandVisual(); e.handled = true; return; }
            if (key.getKeyCode() == STRIKE_KEY_3) { mSelectedIndex = 2; updateHandVisual(); e.handled = true; return; }
            if (key.getKeyCode() == STRIKE_KEY_Q) { throwBlock(); e.handled = true; return; }
        }
    }

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

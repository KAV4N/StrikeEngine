#pragma once
#include "StrikeEngine.h"
#include "WorldGenerator.h"
#include <vector>

class BlockInteraction : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& e) override;

private:
    Strike::Entity mCameraEntity;
    Strike::Entity mHandEntity;

    float mReach     = 400.0f;

    Strike::Entity mHoveredBlock;
    glm::uvec3 mHoveredOriginalColor = glm::uvec3(255);

    
    static constexpr int kBlockTypeCount = 3;
    int mSelectedIndex = 0;

    struct BlockDef {
        BlockType   type;
        const char* modelId;
        const char* label;
    };
    static constexpr BlockDef kBlockDefs[kBlockTypeCount] = {
        { BlockType::Dirt,  "dirt",  "Dirt"  },
        { BlockType::Stone, "stone", "Stone" },
        { BlockType::Grass, "grass", "Grass" },
    };

    BlockType   selectedBlockType() const { return kBlockDefs[mSelectedIndex].type;    }
    const char* selectedModelId()   const { return kBlockDefs[mSelectedIndex].modelId; }

    void cycleBlockType(int delta);
    void updateHandVisual();

    
    static constexpr float kSwingDuration = 0.18f;
    static constexpr float kSwingMid      = 0.45f;

    bool  mSwinging      = false;
    float mSwingProgress = 0.0f;

    static constexpr glm::vec3 kHandBasePos  = {  3.5f, -3.0f, -6.0f };
    static constexpr glm::vec3 kHandBasePeak = {  3.0f, -4.5f, -5.0f };
    static constexpr glm::vec3 kHandBaseRot  = { 20.0f,-35.0f, 15.0f };
    static constexpr glm::vec3 kHandPeakRot  = { 50.0f,-20.0f, 10.0f };
    static constexpr float     kHandBaseScale = 0.7f;

    void triggerSwing();
    void updateSwing();

    
    float     mThrowSpeed      = 1200.0f;
    float     mThrowMass       = 5.0f;
    glm::vec3 mThrowAngularVel = { 200.0f, 120.0f, 80.0f };
    float     mThrowArcBias    = 0.08f;

    static constexpr float kThrownBlockLifetime = 5.0f;

    struct ThrownBlock {
        Strike::Entity entity;
        float          timer = 0.0f;
    };
    std::vector<ThrownBlock> mThrownBlocks;

    void throwBlock();
    void updateThrownBlocks(float deltaTime);

    
    
    
    
    static constexpr const char* kPlaceSoundId = "block_place";
    static constexpr const char* kBreakSoundId = "block_break";

    
    
    Strike::Entity mPlaceSoundEntity;
    Strike::Entity mBreakSoundEntity;

    void playBlockSound(Strike::Entity& soundEntity);
    void loadBlockSounds();

    
    void updateHover();
    void clearHover();
    void breakBlock();
    void placeBlock();
    void notifyBlockDestroyed(const Strike::Entity& entity);
    glm::vec3 dominantNormal(const glm::vec3& normal) const;
};

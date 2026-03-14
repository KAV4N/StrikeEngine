#pragma once
#include "StrikeEngine.h"

class BlockInteraction : public Strike::Script {
public:
    void onStart()                 override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& e) override;

private:
    Strike::Entity mCameraEntity;

    float mReach     = 400.0f;
    float mCubeScale = 0.8f;


    Strike::Entity mHoveredBlock;
    glm::uvec3     mHoveredOriginalColor = glm::uvec3(255);

    void updateHover();
    void clearHover();

    void breakBlock();
    void placeBlock();

    glm::vec3 dominantNormal(const glm::vec3& normal) const;
};
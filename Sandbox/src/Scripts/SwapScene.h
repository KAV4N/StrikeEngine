#pragma once
#include "StrikeEngine.h"

class SwapScene : public StrikeEngine::Script
{
public:
    void onStart() override;
    void onUpdate(float dt) override;

private:
    void doSwap();

    void initGameData();
    void onSceneEnter();
    void onSceneLeave();
    void printGameDataSummary();

    void updateHUD();
    void buildHUD();

private:
    bool mFWasDown = false;
    bool mTWasDown = false;

    float mTimeInScene = 0.0f;
    bool  mInitialized = false;

    StrikeEngine::Entity mHUDLine1;
    StrikeEngine::Entity mHUDLine2;
    StrikeEngine::Entity mHUDLine3;
    StrikeEngine::Entity mHUDLine4;

    static constexpr const char* SCENES[] =
    {
        "Assets/Scenes/HierarchyShowcase.xml",
        "Assets/Scenes/AssetShowcase.xml",
    };

    static constexpr int SCENE_COUNT = 2;
};
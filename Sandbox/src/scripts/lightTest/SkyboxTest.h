#pragma once
#include "StrikeEngine.h"

class SkyboxTest : public Strike::Script
{
public:
    void onStart()  override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& event) override;

private:
    enum class State { None, Sync, Async };

    static constexpr const char* kIdSync  = "skybox_sync";
    static constexpr const char* kIdAsync = "skybox_async";
    static constexpr const char* kPath    = "Assets/Test/skybox";

    State mState = State::None;

    void applyNone();
    void applySync();
    void applyAsync();
    void printState() const;
};
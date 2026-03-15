#include "SkyboxTest.h"

void SkyboxTest::onStart()
{
    STRIKE_INFO("[SkyboxTest] Press 1 = no cubemap, 2 = sync load, 3 = async load");
    printState();
}

void SkyboxTest::onUpdate(float deltaTime)
{
    auto& am = Strike::AssetManager::get();
    if (am.isLoading())
        return;
}

void SkyboxTest::onEvent(Strike::Event& event)
{
    if (event.getEventType() != Strike::EventType::KeyPressed)
        return;

    auto& key = static_cast<Strike::KeyPressedEvent&>(event);
    if (key.getRepeatCount() > 0)
        return;

    switch (key.getKeyCode())
    {
        case STRIKE_KEY_1: applyNone();  break;
        case STRIKE_KEY_2: applySync();  break;
        case STRIKE_KEY_3: applyAsync(); break;
        default: return;
    }

    event.handled = true;
}

// =====================================================================
// State transitions
// =====================================================================

void SkyboxTest::applyNone()
{
    auto& am = Strike::AssetManager::get();
    am.removeAsset(kIdSync);
    am.removeAsset(kIdAsync);

    getEntity().getScene()->setSkybox("");

    mState   = State::None;
    printState();
}

void SkyboxTest::applySync()
{
    auto& am = Strike::AssetManager::get();
    am.removeAsset(kIdSync);
    am.removeAsset(kIdAsync);

    auto cm = am.load<Strike::CubeMap>(kIdSync, kPath);

    getEntity().getScene()->setSkybox(kIdSync);
    mState   = State::Sync;
    printState();
}

void SkyboxTest::applyAsync()
{
    auto& am = Strike::AssetManager::get();
    am.removeAsset(kIdSync);
    am.removeAsset(kIdAsync);


    am.loadAsync<Strike::CubeMap>(kIdAsync, kPath);

    getEntity().getScene()->setSkybox(kIdAsync);

    mState   = State::Async;
    printState();
    STRIKE_INFO("[SkyboxTest] Async cubemap loading...");
}

void SkyboxTest::printState() const
{
    switch (mState)
    {
        case State::None:  STRIKE_INFO("[SkyboxTest] State: no cubemap"); break;
        case State::Sync:  STRIKE_INFO("[SkyboxTest] State: sync");       break;
        case State::Async: STRIKE_INFO("[SkyboxTest] State: async");      break;
    }
}

REGISTER_SCRIPT(SkyboxTest)
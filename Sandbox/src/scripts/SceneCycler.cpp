#include "SceneCycler.h"

void SceneCycler::onStart() {
    mScenes = { "Assets/Scenes/lightTest.xml", "Assets/Scenes/assetTest.xml", "Assets/Scenes/game.xml" };

    auto& gd = Strike::GameData::get();
    if (gd.hasKey("SceneCycler.index"))
        mCurrentIndex = gd.getInt("SceneCycler.index");
}

void SceneCycler::onEvent(Strike::Event& e) {
    if (e.getEventType() != Strike::EventType::KeyPressed) return;

    auto& key = static_cast<Strike::KeyPressedEvent&>(e);
    if (key.getRepeatCount() > 0) return;
    if (key.getKeyCode() != STRIKE_KEY_T) return;

    mCurrentIndex = (mCurrentIndex + 1) % static_cast<int>(mScenes.size());
    Strike::GameData::get().setInt("SceneCycler.index", mCurrentIndex);
    Strike::World::get().loadScene(mScenes[mCurrentIndex]);

    e.handled = true;
}

REGISTER_SCRIPT(SceneCycler)
#include "World.h"
#include <iostream>

namespace StrikeEngine {

    World::World()
    {
    }

    void World::loadScene(const std::filesystem::path& path)
    {
        //mCurrentScene = mSceneLoader->loadScene(path);
    }

    void World::loadSceneAsync(const std::filesystem::path& path)
    {
        //mPendingScene = mSceneLoader->loadSceneAsync(path);
    }

    bool World::isSceneLoading() const
    {
        return mPendingScene.valid() && mPendingScene.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
    }

    void World::update(float dt)
    {
        checkAndSwitchScene();
        if (mCurrentScene && mCurrentScene->isActive()) {
            mCurrentScene->onUpdate(dt);
        }
    }

    void World::onRender()
    {
        if (mCurrentScene && mCurrentScene->isActive()) {
            mCurrentScene->onRender();
        }
    }

    void World::onImGuiRender()
    {
        if (mCurrentScene && mCurrentScene->isActive()) {
            mCurrentScene->onImGuiRender();
        }
    }

    void World::onEvent(Event& e)
    {
        if (mCurrentScene && mCurrentScene->isActive()) {
            mCurrentScene->onEvent(e);
        }
    }

    void World::checkAndSwitchScene()
    {
        if (mPendingScene.valid() && mPendingScene.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            mCurrentScene = mPendingScene.get();
        }
    }

} // namespace StrikeEngine
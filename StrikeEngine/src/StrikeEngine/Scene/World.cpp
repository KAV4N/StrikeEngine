#include "World.h"
#include <thread>

namespace StrikeEngine {

    World::World() {
        mCurrentScene = std::make_unique<Scene>("default_scene", "", "Default Scene");
    }

    void World::loadSceneAsync(const std::string& id, const std::filesystem::path& path, const std::string& name) {
        mPendingScene = loadSceneAsyncImpl(id, path, name);
    }

    std::future<std::unique_ptr<Scene>> World::loadSceneAsyncImpl(const std::string& id, const std::filesystem::path& path, const std::string& name) {
        return std::async(std::launch::async, [id, path, name]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return std::make_unique<Scene>(id, path, name);
            });
    }

    bool World::isSceneLoading() const {
        return mPendingScene.valid() && mPendingScene.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
    }

    void World::update(float dt) {
        checkAndSwitchScene();

        if (mCurrentScene) {
            mCurrentScene->update(dt);
        }
    }

    void World::checkAndSwitchScene() {
        if (mPendingScene.valid() && mPendingScene.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            mCurrentScene = mPendingScene.get();
        }
    }

}
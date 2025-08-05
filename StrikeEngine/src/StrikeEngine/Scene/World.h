#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <future>
#include "Scene.h"

namespace StrikeEngine {

    class World final {
    public:
        World();
        ~World() = default;

        void loadSceneAsync(const std::string& id, const std::filesystem::path& path, const std::string& name = "Untitled");

        Scene* getCurrentScene() const { return mCurrentScene.get(); }

        bool isSceneLoading() const;

        void update(float dt);

    private:
        std::future<std::unique_ptr<Scene>> loadSceneAsyncImpl(const std::string& id, const std::filesystem::path& path, const std::string& name);

        void checkAndSwitchScene();

    private:
        std::unique_ptr<Scene> mCurrentScene;
        std::future<std::unique_ptr<Scene>> mPendingScene;
    };

}
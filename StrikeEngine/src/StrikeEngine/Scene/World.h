#pragma once
#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <future>
#include "Scene.h"
#include "SceneLoader.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {
    class World final {
    public:
        // Singleton access
        static World& get() {
            static World instance;
            return instance;
        }

        void loadScene(const std::filesystem::path& path);
        void loadSceneAsync(const std::filesystem::path& path);
        Scene* getCurrentScene() const { return mCurrentScene.get(); }
        bool isSceneLoading() const;

        // Update methods
        void update(float dt);
        void onRender();
        void onImGuiRender();
        void onEvent(Event& e);

    private:
        World();
        ~World() = default;
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) = delete;
        World& operator=(World&&) = delete;

        void checkAndSwitchScene();

    private:
        std::unique_ptr<Scene> mCurrentScene;
        std::future<std::unique_ptr<Scene>> mPendingScene;
        std::unique_ptr<SceneLoader> mSceneLoader;
    };
}
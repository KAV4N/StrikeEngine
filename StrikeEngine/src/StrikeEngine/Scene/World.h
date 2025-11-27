#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <future>

#include <glm/glm.hpp>

namespace StrikeEngine {

    class Scene;
    class SceneLoader;
    class RenderSystem;
    class ScriptSystem;
    class PhysicsSystem;
    class AudioSystem;
    class Skybox;
    class Sun;
    struct Event;

    class World final {
    public:
        static World& get();

        void loadScene(const std::filesystem::path& path);
        void loadSceneAsync(const std::filesystem::path& path);
        
        Scene* getScene() const;
        bool isLoading() const;

        void onUpdate(float dt);
        void onRender();
        void onEvent(Event& e);
        void resize(uint32_t width, uint32_t height);

    private:
        World();
        ~World() = default;
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) = delete;
        World& operator=(World&&) = delete;

        void checkAndSwitchScene();
        void clearPhysicsWorld();  

    private:
        std::unique_ptr<Scene> mCurrentScene;
        std::future<std::unique_ptr<Scene>> mPendingScene;

        std::unique_ptr<SceneLoader> mSceneLoader;

        std::unique_ptr<RenderSystem> mRenderSystem;
        std::unique_ptr<ScriptSystem> mScriptSystem;
        std::unique_ptr<PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<AudioSystem> mAudioSystem;
    };

}
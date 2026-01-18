#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"

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

        // Unity-style: scene loads at end of frame
        void loadScene(const std::filesystem::path& path);
        
        Scene* getScene() const;

        void onUpdate(float dt);
        void onRender();
        void onEvent(Event& e);
        void resize(uint32_t width, uint32_t height);

        RayHit rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;
        std::vector<RayHit> rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;

        void setGravity(const glm::vec3& gravity);
        glm::vec3 getGravity() const;

    private:
        friend class Entity;
        friend class Scene;
        
        World();
        ~World() = default;
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) = delete;
        World& operator=(World&&) = delete;

        void clearPhysicsWorld();
        void processSceneLoad(); 

    private:
        std::unique_ptr<Scene> mCurrentScene;
        std::filesystem::path mPendingScenePath; 
        bool mSceneLoadPending = false;

        std::unique_ptr<SceneLoader> mSceneLoader;

        std::unique_ptr<RenderSystem> mRenderSystem;
        std::unique_ptr<ScriptSystem> mScriptSystem;
        std::unique_ptr<PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<AudioSystem> mAudioSystem;
    };

}
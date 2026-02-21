#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"

#include <glm/glm.hpp>

namespace Strike {

    class Scene;
    class SceneLoader;
    class RenderSystem;
    class ScriptSystem;
    class PhysicsSystem;
    class AudioSystem;
    class Skybox;
    class Sun;
    class Event;

    /**
     * @brief Structure for Fog settings.
     */
    struct Fog
    {
        float start = 0.0f;
        float end = 100.0f;
        float density = 0.0f;
        glm::uvec3 color = glm::uvec3(255, 255, 255); // default white
    };

    /**
     * @brief The World class manages the current scene and its systems.
     * It handles scene loading, updates, rendering, and event processing.
     * It also provides ray casting functionality and global settings like gravity and fog.
     */
    class World final {
    public:
        static World& get();

        /**
         * @brief Load a scene from the specified file path.
         * @param path The file path to the scene file.
         * @note This initiates the scene loading process; the actual loading happens att the end of the frame.
         * and the new scene becomes active in the next frame.
         */
        void loadScene(const std::filesystem::path& path);
        
        /**
         * @brief Get the currently active scene.
         * @return Pointer to the current Scene object.
         * @note The returned pointer may be nullptr if no scene is loaded.
         */
        Scene* getScene() const;

        /**
         * @brief Perform a ray cast in the physics world.
         * @param origin The starting point of the ray.
         * @param direction The direction of the ray.
         * @param maxDistance The maximum distance the ray should check for collisions.
         * @return A RayHit structure containing information about the first hit, if any.
         * @note If no collision is detected, the returned RayHit will have an invalid entity.
         */
        RayHit rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;

        /**
         * @brief Perform a ray cast in the physics world and return all hits.
         * @param origin The starting point of the ray.
         * @param direction The direction of the ray.
         * @param maxDistance The maximum distance the ray should check for collisions.
         * @return A vector of RayHit structures containing information about all hits along the ray.
         * @note If no collisions are detected, the returned vector will be empty.
         */
        std::vector<RayHit> rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;

        /**
         * @brief Set the global gravity vector for the physics world.
         * @param gravity The gravity vector to set.
        *  @return The current gravity vector of the physics world.
         * @note This affects all physics-enabled entities in the scene.
         */
        void setGravity(const glm::vec3& gravity);

        /**
         * @brief Get the current global gravity vector of the physics world.
         * @return The current gravity vector of the physics world.
         */
        glm::vec3 getGravity() const;


        /**
         * @brief Set the fog settings for the world.
         * @param start The distance at which the fog starts.
         * @param end The distance at which the fog ends.
         * @param density The density of the fog.
         * @param color The color of the fog as a glm::uvec3 (RGB).
         * @note Set density to 0 to disable fog.
         */
        void setFog(float start, float end, float density, const glm::uvec3& color);

        /**
         * @brief Get the current fog settings.
         * @return Const reference to the Fog structure.
         */
        const Fog& getFog() const { return mFog; }

    private:
        friend class Entity;
        friend class Scene;

        friend class Renderer;
        friend class Application;

        World();
        ~World() = default;
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) = delete;
        World& operator=(World&&) = delete;

        void clearPhysicsWorld();
        void processSceneLoad(); 

        void onUpdate(float dt);
        void onRender();
        void onEvent(Event& e);
        void resize(uint32_t width, uint32_t height);

        void shutDownSystems();

    private:
        Fog mFog;

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
#pragma once

#include "StrikeEngine/Events/Event.h"

#include "GraphNode.h"
#include "Sun.h"

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>

namespace Strike {

    class Asset;
    class CubeMap;
    class Entity;
    class PhysicsSystem;

    /**
     * @brief Represents a 3D scene containing entities and components.
     * @note Managed by World and loaded via SceneLoader.
     */
    class Scene final {
    public:
        /**
         * @brief Construct a new Scene object.
         * @param tag for the scene.
         * @param path File path to the scene asset.
         * @note This does not load the scene; for loading use SceneLoader.
         */
        Scene(const std::filesystem::path& path, const std::string& tag = "");
        ~Scene();

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        /**
         * @brief Create a new entity in the scene.
         * @return The created Entity.
         * @note The entity will have no parent (root entity).
         */
        Entity createEntity();

        /**
         * @brief Create a new entity in the scene with a specified parent.
         * @param parent The parent entity.
         * @return The created Entity.
         */
        Entity createEntity(const Entity& parent);

        /**
         * @brief Get an entity by its EnTT handle.
         * @param handle The EnTT entity handle.
         * @return The corresponding Entity.
         * @note Returns an invalid Entity if the handle does not exist.
         */
        Entity getEntityFromHandle(entt::entity handle);

        /**
         * @brief Get an entity by its tag.
         * @param tag The tag of the entity.
         * @return The corresponding Entity.
         * @note Returns an invalid Entity if no entity with the tag exists.
         */
        Entity getEntity(std::string tag);

        /**
         * @brief Set the parent of a child entity.
         * @param child The child entity.
         * @param parent The new parent entity.
         * @note If parent is invalid, the child becomes a root entity.
         */
        void setParent(const Entity& child, const Entity& parent);

        /**
         * @brief Add a child entity to a parent entity.
         * @param parent The parent entity.
         * @param child The child entity to add.
         */
        void addChild(const Entity& parent, const Entity& child);

        /**
         * @brief Set the skybox cubemap for the scene.
         * @param cubeMapId The asset ID of the cubemap to use as the skybox.
         * @note Pass an empty string to remove the skybox.
         */
        void setSkybox(const std::string& cubeMapId);

        /**
         * @brief Check if the scene has a skybox set.
         * @return True if a skybox is set, false otherwise.
         */
        bool hasSkybox() const;

        /**
         * @brief Get the asset ID of the skybox cubemap.
         * @return The asset ID of the skybox cubemap.
         */
        const std::string& getSkyboxId() const { return mSkyboxCubeMapId; }

        /**
         * @brief Get the skybox cubemap asset.
         * @return Shared pointer to the CubeMap asset.
         * @note Returns nullptr if no skybox is set.
         */
        std::shared_ptr<CubeMap> getSkybox() const;
        
       
      /**
       * @brief Destroy an entity and its associated graph node.
       * @param entity The EnTT entity handle to destroy.
       * @note This will also remove the entity from its parent's children list if applicable.
       */
        void destroy(entt::entity handle);

        /**
         * @brief Check if one entity is an ancestor of another in the scene graph.
         * @param ancestor The potential ancestor entity.
         * @param descendant The potential descendant entity.
         * @return True if 'ancestor' is an ancestor of 'descendant', false otherwise
         */
        bool isAncestor(const Entity& ancestor, const Entity& descendant) const;

        /**
         * @brief Check if one entity is a descendant of another in the scene graph.
         * @param descendant The potential descendant entity.
         * @param ancestor The potential ancestor entity.
         * @return True if 'descendant' is a descendant of 'ancestor', false otherwise
         */
        bool isDescendant(const Entity& descendant, const Entity& ancestor) const;

        /**
         * @brief Get the EnTT registry for direct ECS operations.
         * @return Reference to the EnTT registry.
         * @note Use with caution; prefer using Entity methods when possible.
         */
        entt::registry& getRegistry() { return mRegistry; }

        /**
         * @brief Get the EnTT registry for direct ECS operations (const version).
         * @return Const reference to the EnTT registry.
         * @note Use with caution; prefer using Entity methods when possible.
         */
        const entt::registry& getRegistry() const { return mRegistry; }

        /**
         * @brief Add a component to an entity.
         * @tparam Component The type of the component to add.
         * @param entity The EnTT entity handle.
         * @param args Arguments to forward to the component constructor.
         * @return Reference to the added component.
         */
        template<typename Component, typename... Args>
        decltype(auto) addComponent(entt::entity entity, Args&&... args) {
            return mRegistry.emplace<Component>(entity, std::forward<Args>(args)...);
        }

        /**
         * @brief Remove a component from an entity.
         * @tparam Component The type of the component to remove.
         * @param entity The EnTT entity handle.
         */
        template<typename Component>
        void removeComponent(entt::entity entity) {
            mRegistry.remove<Component>(entity);
        }

        /**
         * @brief Get a component from an entity.
         * @tparam Component The type of the component to get.
         * @param entity The EnTT entity handle.
         * @return Reference to the requested component.
         */
        template<typename Component>
        Component& getComponent(entt::entity entity) {
            return mRegistry.get<Component>(entity);
        }

        /**
         * @brief Get a const component from an entity.
         * @tparam Component The type of the component to get.
         * @param entity The EnTT entity handle.
         * @return Reference to the requested component.
         */
        template<typename Component>
        const Component& getComponent(entt::entity entity) const {
            return mRegistry.get<Component>(entity);
        }

        /**
         * @brief Check if an entity has a specific component.
         * @tparam Component The type of the component to check for.
         * @param entity The EnTT entity handle.
         * @return True if the component exists, false otherwise.
         */
        template<typename Component>
        bool hasComponent(entt::entity entity) const {
            return mRegistry.all_of<Component>(entity);
        }

        /**
         * @brief Get the Sun object representing the scene's sunlight.
         * @return Reference to the Sun object.
         */
        Sun& getSun() { return mSun; }

        /**
         * @brief Get the Sun object representing the scene's sunlight (const version).
         * @return Const reference to the Sun object.
         */
        const Sun& getSun() const { return mSun; }

        /**
         * @brief Get the unique identifier of the scene.
         * @return The scene's unique identifier.
         */
        const std::string& getTag() { return mId; }

    private:
        friend class Entity;
        friend class World;
        
        void shutdown();

                
        void onUpdate(float dt);
        void onRender();

        // Internal graph node access (for Entity class only)
        std::shared_ptr<GraphNode> getGraphNode(entt::entity entity);
        const std::shared_ptr<GraphNode> getGraphNode(entt::entity entity) const;

        void updateNodeTransforms(std::shared_ptr<GraphNode> node, bool parentDirty = false);

        void onPhysicsComponentDestroy(entt::registry& registry, entt::entity entity);
        void onAudioSourceDestroy(entt::registry& registry, entt::entity entity);
        void setupComponentProtection();

        void setPhysicsSystem(PhysicsSystem* physicsSystem);

    private:
        friend class AudioSystem;

        std::string mId;
        entt::registry mRegistry;
        Sun mSun; 

        std::string mSkyboxCubeMapId;
        
        // Graph node storage
        std::unordered_map<entt::entity, std::shared_ptr<GraphNode>> mGraphNodes;
    };

} 
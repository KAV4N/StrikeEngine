#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "Scene.h"
#include "StrikeEngine/Core/Log.h"


namespace Strike {

    class Scene;


    /**
     * @brief Represents an entity within a scene, providing ECS component management and transform operations.
     * @note Entities are managed by the Scene and use EnTT under the hood.
     */
    class Entity final {
    public:
        Entity() = default;

        /**
         * @brief Construct a new Entity object.
         * @param handle The EnTT entity handle.
         * @param scene Pointer to the owning Scene.
         */
        Entity(entt::entity handle, Scene* scene);
        ~Entity() = default;

        // Copy / Move
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;
        Entity(Entity&&) = default;
        Entity& operator=(Entity&&) = default;

        /**
         * @brief Check if the entity is valid (exists in the scene).
         * @return True if valid, false otherwise.
         * @note An entity is valid if its handle is not null and it exists in the scene's registry.
         */
        bool isValid() const;

        /**
         * @brief Check if the entity is active.
         * @return True if active, false otherwise.
         */
        bool isActive() const;

        /**
         * @brief Set the active state of the entity.
         * @param active True to activate, false to deactivate.
         */
        void setActive(bool active);

        // ===============================
        // ECS component operations
        // ===============================

        /**
         * @brief Add a component to the entity.
         * @tparam T The type of the component to add.
         * @param args Arguments to forward to the component constructor.
         * @return Reference to the added component.
         */
        template<typename T, typename... Args>
        T& addComponent(Args&&... args);

        /**
         * @brief Remove a component from the entity.
         * @tparam T The type of the component to remove.
         */
        template<typename T>
        void removeComponent();

        /**
         * @brief Get a component from the entity.
         * @tparam T The type of the component to get.
         * @return Reference to the requested component.
         */
        template<typename T>
        T& getComponent();

        /**
         * @brief Get a const component from the entity.
         * @tparam T The type of the component to get.
         * @return Reference to the requested component.
         */
        template<typename T>
        const T& getComponent() const;

        /**
         * @brief Check if the entity has a specific component.
         * @tparam T The type of the component to check for.
         * @return True if the component exists, false otherwise.
         */
        template<typename T>
        bool hasComponent() const;

        /**
         * @brief Get or add a component to the entity.
         * @tparam T The type of the component.
         * @tparam Args Constructor arguments for the component if it needs to be added.
         * @param args Arguments to forward to the component constructor if adding.
         * @return Reference to the existing or newly added component.
         */
        template<typename T, typename... Args>
        T& getOrAddComponent(Args&&... args);

        // ===============================
        // GraphNode passthrough
        // ===============================
        /**
         * @brief Set the tag of the entity.
         * @param tag The tag string to set.
         */
        void setTag(const std::string& tag);

        /**
         * @brief Get the tag of the entity.
         * @return The tag string.
         */
        const std::string& getTag() const;

        // Local transform
        /**
         * @brief Get the local position of the entity.
         * @return Local position as glm::vec3.
         */
        const glm::vec3& getPosition() const;

        /**
         * @brief Set the local position of the entity.
         * @param position The new local position.
         */
        void setPosition(const glm::vec3& position);

        /**
         * @brief Move the entity by a delta offset to direction where Entity is heading.
         * @param delta The offset to move by.
         */
        void move(const glm::vec3& delta);

        /**
         * @brief Get the local rotation of the entity as a quaternion.
         * @return Local rotation as glm::quat.
         */
        const glm::quat& getRotation() const;

        /**
         * @brief Set the local rotation of the entity.
         * @param rotation The new local rotation.
         */
        void setRotation(const glm::quat& rotation);

        /**
         * @brief Get the local rotation of the entity as Euler angles (degrees).
         * @return Local rotation as glm::vec3 (x, y, z).
         */
        glm::vec3 getEulerAngles() const;

        /**
         * @brief Set the local rotation of the entity using Euler angles (degrees).
         * @param angles The new Euler angles (x, y, z).
         */
        void setEulerAngles(const glm::vec3& angles);

        /**
         * @brief Get the local scale of the entity.
         * @return Local scale as glm::vec3.
         */
        const glm::vec3& getScale() const;

        /**
         * @brief Set the local scale of the entity.
         * @param scale The new local scale.
         */
        void setScale(const glm::vec3& scale);

        /**
         * @brief Rotate the entity by given Euler angles (degrees).
         * @param angles The Euler angles to rotate by (x, y, z).
         */
        void rotate(const glm::vec3& angles);

        /**
         * @brief Rotate the entity by a quaternion representing an angle and axis.
         * @param angleDegrees The angle in degrees.
         */
        void rotateQuaternion(float angleDegrees, const glm::vec3& axis);

        // World transform
        /**
         * @brief Get the world position of the entity.
         * @return World position as glm::vec3.
         */
        glm::vec3 getWorldPosition() const;

        /**
         * @brief Set the world position of the entity.
         * @param position The new world position.
         */
        void setWorldPosition(const glm::vec3& position);

        /**
         * @brief Get the world rotation of the entity.
         * @return World rotation as glm::quat.
         */
        glm::quat getWorldRotation() const;

        /**
         * @brief Set the world rotation of the entity.
         * @param rotation The new world rotation.
         */
        void setWorldRotation(const glm::quat& rotation);

        /**
         * @brief Get the world rotation of the entity as Euler angles (degrees).
         * @return World rotation as glm::vec3 (x, y, z).
         */
        glm::vec3 getWorldEulerAngles() const;

        /**
         * @brief Set the world rotation of the entity using Euler angles (degrees).
         * @param angles The new world Euler angles (x, y, z).
         */
        void setWorldEulerAngles(const glm::vec3& angles);

        /**
         * @brief Get the world scale of the entity.
         * @return World scale as glm::vec3.
         */
        glm::vec3 getWorldScale() const;

        // Direction vectors
        /**
         * @brief Get the forward direction vector of the entity.
         * @return Forward direction as glm::vec3.
         */
        glm::vec3 getForward() const;

        /**
         * @brief Get the right direction vector of the entity.
         * @return Right direction as glm::vec3.
         */
        glm::vec3 getRight() const;

        /**
         * @brief Get the up direction vector of the entity.
         * @return Up direction as glm::vec3.
         */
        glm::vec3 getUp() const;

        // Matrices

        /**
         * @brief Get the local transformation matrix of the entity.
         * @return Local transformation matrix as glm::mat4.
         */
        const glm::mat4& getLocalMatrix() const;

        /**
         * @brief Get the world transformation matrix of the entity.
         * @return World transformation matrix as glm::mat4.
         */
        const glm::mat4& getWorldMatrix() const;

        // Hierarchy
        /**
         * @brief Check if the entity is a root entity (has no parent).
         * @return True if root, false otherwise.
         */
        bool isRoot() const;

        /**
         * @brief Get the parent entity.
         * @return The parent Entity.
         */
        Entity getParent() const;

        /**
         * @brief Set the parent entity.
         * @param parent The new parent Entity.
         * @note If parent is invalid, the entity becomes a root entity.
         */
        void setParent(const Entity& parent);

        /**
         * @brief Remove the entity from its parent.
         * @note if parent is invalid, no action is taken.
         * 
         */
        void removeFromParent();

        /**
         * @brief Add a child entity to this entity.
         * @param child The child Entity to add.
         * @note if child is invalid, no action is taken.
         */
        void addChild(const Entity& child);

        /**
         * @brief Get the child entities of this entity.
         * @return Vector of child Entities.
         */
        std::vector<Entity> getChildren() const;

        // Physics
        /**
         * @brief Get a list of entities currently colliding with this entity.
         * @return Vector of colliding Entities.
         */
        std::vector<Entity> getCollidingEntities() const;

        /**
         * @brief Check if this entity is colliding with another entity.
         * @param other The other Entity to check collision with.
         * @return True if colliding, false otherwise.
         */
        bool isCollidingWith(const Entity& other) const;

        // Lifecycle
        /**
         * @brief Destroy the entity, removing it from the scene.
         */
        void destroy() const;

        // Operators
        bool operator==(const Entity& other) const {
            return mHandle == other.mHandle && mScene == other.mScene;
        }

        bool operator!=(const Entity& other) const {
            return !(*this == other);
        }

        /**
         * @brief Bool conversion operator to check validity.
         * @return True if the entity is valid, false otherwise.
         */
        operator bool() const {
            return isValid();
        }

        // Accessors
        /**
         * @brief Get the EnTT entity handle.
         * @return The EnTT entity handle.
         * @note Use with caution; prefer using Entity methods when possible.
         */
        entt::entity getHandle() const { return mHandle; }

        /**
         * @brief Get the owning Scene of the entity.
         * @return Pointer to the owning Scene.
         */
        Scene* getScene() const { return mScene; }

    private:
        friend class Scene;
        friend struct std::hash<Entity>;

        entt::entity mHandle{ entt::null };
        Scene* mScene{ nullptr };
    };

    // ===============================
    // Template implementations
    // ===============================
    template<typename T, typename... Args>
    inline T& Entity::addComponent(Args&&... args) {
        STRIKE_ASSERT(isValid(), "Entity::addComponent called on invalid entity");
        return mScene->addComponent<T>(mHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    inline void Entity::removeComponent() {
        STRIKE_ASSERT(isValid(), "Entity::removeComponent called on invalid entity");   
        mScene->removeComponent<T>(mHandle);
    }

    template<typename T>
    inline T& Entity::getComponent() {
        STRIKE_ASSERT(isValid(), "Entity::getComponent called on invalid entity");
        return mScene->getComponent<T>(mHandle);
    }

    template<typename T>
    inline const T& Entity::getComponent() const {
        STRIKE_ASSERT(isValid(), "Entity::getComponent called on invalid entity");
        return mScene->getComponent<T>(mHandle);
    }

    template<typename T>
    inline bool Entity::hasComponent() const {
        return isValid() && mScene->hasComponent<T>(mHandle);
    }

    template<typename T, typename... Args>
    inline T& Entity::getOrAddComponent(Args&&... args) {
        return hasComponent<T>()
            ? getComponent<T>()
            : addComponent<T>(std::forward<Args>(args)...);
    }

} 

namespace std {
    template<>
    struct hash<Strike::Entity> {
        std::size_t operator()(const Strike::Entity& e) const noexcept {
            return std::hash<entt::entity>()(e.getHandle());
        }
    };
}

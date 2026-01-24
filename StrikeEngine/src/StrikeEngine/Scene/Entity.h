#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "Scene.h"
#include "StrikeEngine/Core/Log.h"


namespace StrikeEngine {

    class Scene;

    class Entity final {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        ~Entity() = default;

        // Copy / Move
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;
        Entity(Entity&&) = default;
        Entity& operator=(Entity&&) = default;

        // Validity
        bool isValid() const;
        bool isActive() const;
        void setActive(bool active);

        // ===============================
        // ECS component operations
        // ===============================
        template<typename T, typename... Args>
        T& addComponent(Args&&... args);

        template<typename T>
        void removeComponent();

        template<typename T>
        T& getComponent();

        template<typename T>
        const T& getComponent() const;

        template<typename T>
        bool hasComponent() const;

        template<typename T, typename... Args>
        T& getOrAddComponent(Args&&... args);

        // ===============================
        // GraphNode passthrough
        // ===============================
        void setTag(const std::string& tag);
        const std::string& getTag() const;

        // Local transform
        const glm::vec3& getPosition() const;
        void setPosition(const glm::vec3& position);
        void move(const glm::vec3& delta);

        const glm::quat& getRotation() const;
        void setRotation(const glm::quat& rotation);

        glm::vec3 getEulerAngles() const;
        void setEulerAngles(const glm::vec3& angles);

        const glm::vec3& getScale() const;
        void setScale(const glm::vec3& scale);

        void rotate(const glm::vec3& angles);
        void rotateQuaternion(float angleDegrees, const glm::vec3& axis);

        // World transform
        glm::vec3 getWorldPosition() const;
        void setWorldPosition(const glm::vec3& position);

        glm::quat getWorldRotation() const;
        void setWorldRotation(const glm::quat& rotation);

        glm::vec3 getWorldEulerAngles() const;
        void setWorldEulerAngles(const glm::vec3& angles);

        glm::vec3 getWorldScale() const;

        // Direction vectors
        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;

        // Matrices
        const glm::mat4& getLocalMatrix() const;
        const glm::mat4& getWorldMatrix() const;

        // Hierarchy
        bool isRoot() const;
        Entity getParent() const;

        void setParent(Entity parent);
        void removeFromParent();
        void addChild(Entity child);

        std::vector<Entity> getChildren() const;

        // Physics
        std::vector<Entity> getCollidingEntities() const;
        bool isCollidingWith(const Entity& other) const;

        // Lifecycle
        void destroy() const;

        // Operators
        bool operator==(const Entity& other) const {
            return mHandle == other.mHandle && mScene == other.mScene;
        }

        bool operator!=(const Entity& other) const {
            return !(*this == other);
        }

        operator bool() const {
            return isValid();
        }

        // Accessors
        entt::entity getHandle() const { return mHandle; }
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

} // namespace StrikeEngine

namespace std {
    template<>
    struct hash<StrikeEngine::Entity> {
        std::size_t operator()(const StrikeEngine::Entity& e) const noexcept {
            return std::hash<entt::entity>()(e.getHandle());
        }
    };
}

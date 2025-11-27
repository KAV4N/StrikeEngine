#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>


#include "Scene.h"

namespace StrikeEngine {

    class Scene;
    class HierarchyComponent;
    class TransformComponent;

    class Entity final {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);

        // Copy and move
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;
        Entity(Entity&&) = default;
        Entity& operator=(Entity&&) = default;

        // Validity
        bool isValid() const;

        // Core component operations
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

        // Essential transform shortcuts
        TransformComponent& getTransform();
        const TransformComponent& getTransform() const;

        // Essential component accessors
        HierarchyComponent& getHierarchy();
        const HierarchyComponent& getHierarchy() const;
        
        void setTag(const std::string& tag);
        const std::string& getTag() const;

        // Consolidated transform controls
        void setPosition(const glm::vec3& position) { getTransform().setPosition(position); }
        void move(const glm::vec3& offset) { getTransform().move(offset); }
        
        void setRotation(const glm::quat& rotation) { getTransform().setRotation(rotation); }
        void setRotationEuler(const glm::vec3& rotation) { getTransform().setRotationEuler(rotation); }
        void rotateEuler(const glm::vec3& anglesDegrees) { getTransform().rotateEuler(anglesDegrees); }
        
        void setScale(const glm::vec3& scale) { getTransform().setScale(scale); }
        void scaleBy(const glm::vec3& factor) { getTransform().scale(factor); }

        // Essential getters
        glm::vec3 getPosition() const { return getTransform().getPosition(); }
        glm::quat getRotation() const { return getTransform().getRotation(); }
        glm::vec3 getEulerRotation() const { return getTransform().getRotationEuler(); }
        glm::vec3 getScale() const { return getTransform().getScale(); }
        glm::mat4 getWorldMatrix() const { return getTransform().getWorldMatrix(); }

        // Essential hierarchy operations
        bool isRoot() const;
        Entity getParent() const;

        void setParent(Entity parent);
        void addChild(Entity child);
        void destroyChildren(); 

        std::vector<Entity> getChildren() const;
        size_t getChildCount() const;
        bool hasChildren() const;

        // Operators
        bool operator==(const Entity& other) const { return mHandle == other.mHandle && mScene == other.mScene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }
        operator bool() const { return isValid(); }

        // Accessors
        entt::entity getHandle() const { return mHandle; }
        Scene* getScene() const { return mScene; }

    private:
        friend class Scene;
        friend struct std::hash<Entity>;

        entt::entity mHandle = entt::null;
        Scene* mScene = nullptr;
    };

    // Entity method implementations
    template<typename T, typename... Args>
    inline T& Entity::addComponent(Args&&... args) {
        if (!isValid()) {
            throw std::runtime_error("Invalid entity or scene");
        }
        return mScene->addComponent<T>(mHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    inline void Entity::removeComponent() {
        if (!isValid()) {
            return;
        }
        mScene->removeComponent<T>(mHandle);
    }

    template<typename T>
    inline T& Entity::getComponent() {
        if (!isValid()) {
            throw std::runtime_error("Invalid entity or scene");
        }
        return mScene->getComponent<T>(mHandle);
    }

    template<typename T>
    inline const T& Entity::getComponent() const {
        std::string name = T::getStaticTypeName();
        if (!isValid()) {
            throw std::runtime_error("Invalid entity or scene");
        }
        return mScene->getComponent<T>(mHandle);
    }

    template<typename T>
    inline bool Entity::hasComponent() const {
        if (!isValid()) {
            return false;
        }
        return mScene->hasComponent<T>(mHandle);
    }

    template<typename T, typename... Args>
    inline T& Entity::getOrAddComponent(Args&&... args) {
        if (hasComponent<T>()) {
            return getComponent<T>();
        }
        return addComponent<T>(std::forward<Args>(args)...);
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
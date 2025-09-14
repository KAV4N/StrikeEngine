#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace StrikeEngine {
    class Scene;
    class SceneGraph;

    class Entity final {
    public:
        Entity() = default;
        Entity(Scene& scene, entt::entity handle);

        // Validity
        bool isValid() const;
        operator bool() const { return isValid(); }

        entt::entity getHandle() const { return handle; }
        std::string getName() const;
        std::string getId() const;


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

        void setPosition(const glm::vec3& position);
        void setRotation(const glm::quat& rotation);
        void setScale(const glm::vec3& scale);

        glm::vec3 getPosition() const;
        glm::quat getRotation() const;
        glm::vec3 getScale() const;

        glm::vec3 getWorldPosition() const;
        glm::quat getWorldRotation() const;
        glm::vec3 getWorldScale() const;
        glm::mat4 getWorldMatrix() const;

        void setParent(Entity parent);
        void removeParent();
        Entity getParent() const;
        std::vector<Entity> getChildren() const;

        void addChild(Entity child);
        void removeChild(Entity child);

        bool isAncestorOf(Entity other) const;
        bool isDescendantOf(Entity other) const;
        bool isRoot() const;
        bool isSceneRoot() const;
        bool isActive() const;

        bool operator==(const Entity& other) const { return handle == other.handle && scene == other.scene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

    private:
        friend class Scene;

        Scene* scene = nullptr;
        entt::entity handle = entt::null;

        entt::registry& getRegistry() const;
        SceneGraph& getSceneGraph() const;
    };

    
    template<typename T, typename... Args>
    T& Entity::addComponent(Args&&... args) {
        return getRegistry().emplace<T>(handle, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::removeComponent() {
        getRegistry().remove<T>(handle);
    }

    template<typename T>
    T& Entity::getComponent() {
        return getRegistry().get<T>(handle);
    }

    template<typename T>
    const T& Entity::getComponent() const {
        return getRegistry().get<T>(handle);
    }

    template<typename T>
    bool Entity::hasComponent() const {
        return getRegistry().all_of<T>(handle);
    }

    template<typename T, typename... Args>
    T& Entity::getOrAddComponent(Args&&... args) {
        if (hasComponent<T>()) {
            return getComponent<T>();
        }
        return addComponent<T>(std::forward<Args>(args)...);
    }
}

// Updated hash function to avoid recursion
namespace std {
    template <>
    struct hash<StrikeEngine::Entity> {
        std::size_t operator()(const StrikeEngine::Entity& entity) const noexcept {
            return std::hash<entt::entity>{}(entity.getHandle());
        }
    };
}
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace StrikeEngine {

    class SceneGraph;
    class Scene;

    class Entity final {
    public:
        static Entity create(Scene* scene, const std::string& id, const std::string& parentId = "");
        static Entity get(Scene* scene, const std::string& id);

        Entity() = default;
        ~Entity() = default;

        // Copy constructor and assignment
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;

        // Move constructor and assignment
        Entity(Entity&&) = default;
        Entity& operator=(Entity&&) = default;

        // Validity
        bool isValid() const;

        void setName(const std::string& name);
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

        // Position controls
        void setPosition(const glm::vec3& position);
        void setPositionX(float x);
        void setPositionY(float y);
        void setPositionZ(float z);
        void move(const glm::vec3& offset);
        void moveX(float x);
        void moveY(float y);
        void moveZ(float z);
        glm::vec3 getPosition() const;
        float getPositionX() const;
        float getPositionY() const;
        float getPositionZ() const;

        // Rotation controls
        void setRotation(const glm::quat& rotation);
        void setEulerRotation(const glm::vec3& rotation);
        void setRotationX(float angleDegrees);
        void setRotationY(float angleDegrees);
        void setRotationZ(float angleDegrees);
        void rotate(const glm::quat& rotation);
        void rotateEuler(const glm::vec3& anglesDegrees);
        void rotateX(float angleDegrees);
        void rotateY(float angleDegrees);
        void rotateZ(float angleDegrees);
        glm::quat getRotation() const;
        glm::vec3 getEulerRotation() const;
        float getRotationX() const;
        float getRotationY() const;
        float getRotationZ() const;

        // Scale controls
        void setScale(const glm::vec3& scale);
        void setScaleX(float x);
        void setScaleY(float y);
        void setScaleZ(float z);
        void scaleBy(const glm::vec3& factor);
        void scaleByX(float x);
        void scaleByY(float y);
        void scaleByZ(float z);
        glm::vec3 getScale() const;
        float getScaleX() const;
        float getScaleY() const;
        float getScaleZ() const;

        glm::mat4 getLocalMatrix() const;
        glm::mat4 getWorldMatrix() const;

        void setParent(Entity parent);
        Entity getParent() const;
        std::vector<Entity> getChildren() const;

        void addChild(Entity child);
        void removeChild(Entity child);

        bool isAncestorOf(Entity other) const;
        bool isDescendantOf(Entity other) const;
        bool isRoot() const;
        bool isActive() const;

        operator bool() const { return isValid(); }

        bool operator!=(const Entity& other) const { return !(*this == other); }
        bool operator==(const Entity& other) const { return mId == other.mId && mSceneGraph == other.mSceneGraph; }

        Scene* getScene() const;
        SceneGraph* getSceneGraph() const;

    private:
        entt::registry& Entity::getRegistry() const;

    private:
        friend class Scene;
        friend class SceneGraph;
        friend struct std::hash<Entity>;

        Scene* mScene = nullptr;
        SceneGraph* mSceneGraph = nullptr;
        std::string mId;
        std::string mName = "";
        entt::entity mHandle = entt::null;
    };

    template<typename T, typename... Args>
    T& Entity::addComponent(Args&&... args) {
        if (!isValid()) {
            throw std::runtime_error("Invalid entity or scene graph");
        }
        return getRegistry().emplace<T>(mHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::removeComponent() {
        if (!isValid()) {
            return;
        }
        getRegistry().remove<T>(mHandle);
    }

    template<typename T>
    T& Entity::getComponent() {
        if (!isValid()) {
            throw std::runtime_error("Invalid entity or scene graph");
        }
        return getRegistry().get<T>(mHandle);
    }

    template<typename T>
    const T& Entity::getComponent() const {
        if (!isValid()) {
            throw std::runtime_error("Invalid entity or scene graph");
        }
        return getRegistry().get<T>(mHandle);
    }

    template<typename T>
    bool Entity::hasComponent() const {
        if (!isValid()) {
            return false;
        }
        return getRegistry().all_of<T>(mHandle);
    }

    template<typename T, typename... Args>
    T& Entity::getOrAddComponent(Args&&... args) {
        if (hasComponent<T>()) {
            return getComponent<T>();
        }
        return addComponent<T>(std::forward<Args>(args)...);
    }
}

namespace std {
    template<>
    struct hash<StrikeEngine::Entity> {
        std::size_t operator()(const StrikeEngine::Entity& e) const noexcept {
            return std::hash<std::string>()(e.getId());
        }
    };
}
#include "Entity.h"
#include "Scene.h"
#include "SceneGraph.h"

namespace StrikeEngine {

    Entity::Entity(Scene& scene, entt::entity handle)
        : scene(&scene), handle(handle) {
    }

    bool Entity::isValid() const {
        return scene != nullptr && handle != entt::null && scene->isEntityValid(*this);
    }

    std::string Entity::getName() const {
        if (!isValid()) return "";
        return getSceneGraph().getEntityName(*this);
    }

    std::string Entity::getId() const {
        if (!isValid()) return "";
        return getSceneGraph().getEntityId(*this);
    }

    void Entity::setPosition(const glm::vec3& position) {
        if (isValid()) {
            getSceneGraph().setPosition(*this, position);
        }
    }

    void Entity::setRotation(const glm::quat& rotation) {
        if (isValid()) {
            getSceneGraph().setRotation(*this, rotation);
        }
    }

    void Entity::setScale(const glm::vec3& scale) {
        if (isValid()) {
            getSceneGraph().setScale(*this, scale);
        }
    }

    glm::vec3 Entity::getPosition() const {
        if (!isValid()) return glm::vec3(0.0f);
        return getSceneGraph().getPosition(*this);
    }

    glm::quat Entity::getRotation() const {
        if (!isValid()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return getSceneGraph().getRotation(*this);
    }

    glm::vec3 Entity::getScale() const {
        if (!isValid()) return glm::vec3(1.0f);
        return getSceneGraph().getScale(*this);
    }

    glm::vec3 Entity::getWorldPosition() const {
        if (!isValid()) return glm::vec3(0.0f);
        return getSceneGraph().getWorldPosition(*this);
    }

    glm::quat Entity::getWorldRotation() const {
        if (!isValid()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return getSceneGraph().getWorldRotation(*this);
    }

    glm::vec3 Entity::getWorldScale() const {
        if (!isValid()) return glm::vec3(1.0f);
        return getSceneGraph().getWorldScale(*this);
    }

    glm::mat4 Entity::getWorldMatrix() const {
        if (!isValid()) return glm::mat4(1.0f);
        return getSceneGraph().getWorldMatrix(*this);
    }

    void Entity::setParent(Entity parent) {
        if (isValid() && parent.isValid() && !isSceneRoot()) {
            getSceneGraph().setParent(*this, parent);
        }
    }

    void Entity::removeParent() {
        if (isValid() && !isSceneRoot()) {
            getSceneGraph().removeParent(*this);
        }
    }

    Entity Entity::getParent() const {
        if (!isValid()) return Entity();
        return getSceneGraph().getParent(*this);
    }

    std::vector<Entity> Entity::getChildren() const {
        if (!isValid()) return {};
        return getSceneGraph().getChildren(*this);
    }

    void Entity::addChild(Entity child) {
        if (isValid() && child.isValid() && !child.isSceneRoot()) {
            getSceneGraph().addChild(*this, child);
        }
    }

    void Entity::removeChild(Entity child) {
        if (isValid() && child.isValid() && !child.isSceneRoot()) {
            getSceneGraph().removeChild(*this, child);
        }
    }

    bool Entity::isAncestorOf(Entity other) const {
        if (!isValid() || !other.isValid()) return false;
        return getSceneGraph().isAncestor(*this, other);
    }

    bool Entity::isDescendantOf(Entity other) const {
        if (!isValid() || !other.isValid()) return false;
        return getSceneGraph().isDescendant(*this, other);
    }

    bool Entity::isRoot() const {
        if (!isValid()) return false;
        Entity parent = getSceneGraph().getParent(*this);
        return !parent.isValid();
    }

    bool Entity::isSceneRoot() const {
        if (!isValid()) return false;
        return scene->isRootEntity(*this);
    }

    bool Entity::isActive() const {
        if (!isValid()) return false;
        return getSceneGraph().isActive(*this);
    }

    entt::registry& Entity::getRegistry() const {
        return scene->getRegistry();
    }

    SceneGraph& Entity::getSceneGraph() const {
        return scene->getSceneGraph();
    }

}
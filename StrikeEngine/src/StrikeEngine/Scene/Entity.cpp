#include "Entity.h"
#include "Scene.h"
#include "SceneGraph.h"
#include <iostream>
#include <stdexcept>

namespace StrikeEngine {

    Entity Entity::create(Scene* scene, const std::string& id, const std::string& parentId) {
        if (!scene || !scene->getSceneGraph()) {
            throw std::runtime_error("Invalid scene or scene graph");
        }
        SceneGraph* sceneGraph = scene->getSceneGraph();

        return sceneGraph->createEntity(id, parentId);
    }

    Entity Entity::get(Scene* scene, const std::string& id) {
        if (!scene || !scene->getSceneGraph()) {
            return Entity();
        }
        SceneGraph* sceneGraph = scene->getSceneGraph();

        return sceneGraph->getEntity(id);
    }

    bool Entity::isValid() const {
        return mScene != nullptr &&
            mSceneGraph != nullptr &&
            mHandle != entt::null &&
            mSceneGraph->mRegistry.valid(mHandle) &&
            !mId.empty();
    }

    entt::registry& Entity::getRegistry() const {
        return mSceneGraph->mRegistry;
    }

    void Entity::setName(const std::string& name) {
        if (!isValid()) {
            return;
        }
        mName = name;
    }

    std::string Entity::getName() const {
        return mName;
    }

    std::string Entity::getId() const {
        return mId;
    }

    void Entity::setPosition(const glm::vec3& position) {
        if (!isValid() || !mSceneGraph) {
            return;
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setPosition(position);
        }
    }

    void Entity::setRotation(const glm::quat& rotation) {
        if (!isValid() || !mSceneGraph) {
            return;
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setRotation(rotation);
        }
    }

    void Entity::setEulerRotation(const glm::vec3& rotation) {
        if (!isValid() || !mSceneGraph) {
            return;
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setEulerRotation(rotation);
        }
    }

    void Entity::setScale(const glm::vec3& scale) {
        if (!isValid() || !mSceneGraph) {
            return;
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setScale(scale);
        }
    }

    glm::vec3 Entity::getPosition() const {
        if (!isValid() || !mSceneGraph) {
            return glm::vec3(0.0f);
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getPosition();
        }
        return glm::vec3(0.0f);
    }

    glm::quat Entity::getRotation() const {
        if (!isValid() || !mSceneGraph) {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getRotation();
        }
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Entity::getEulerRotation() const {
        if (!isValid() || !mSceneGraph) {
            return glm::vec3( 0.0f, 0.0f, 0.0f);
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getEulerRotation();
        }
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Entity::getScale() const {
        if (!isValid() || !mSceneGraph) {
            return glm::vec3(1.0f);
        }

        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getScale();
        }
        return glm::vec3(1.0f);
    }

    glm::mat4 Entity::getLocalMatrix() const {
        if (!isValid() || !mSceneGraph) {
            return glm::mat4(1.0f);
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getLocalMatrix();
        }
        return glm::mat4(1.0f);
    }
    glm::mat4 Entity::getWorldMatrix() const {
        if (!isValid() || !mSceneGraph) {
            return glm::mat4(1.0f);
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getWorldMatrix();
        }
        return glm::mat4(1.0f);
    }

    void Entity::setParent(Entity parent) {
        if (!isValid() || !mSceneGraph) {
            return;
        }

        if (parent.isValid()) {
            mSceneGraph->setParent(mId, parent.getId());
        }
    }

    Entity Entity::getParent() const {
        if (!isValid() || !mSceneGraph) {
            return Entity();
        }

        return mSceneGraph->getParent(mId);
    }

    std::vector<Entity> Entity::getChildren() const {
        if (!isValid() || !mSceneGraph) {
            return std::vector<Entity>();
        }

        return mSceneGraph->getChildren(mId);
    }

    void Entity::addChild(Entity child) {
        if (!isValid() || !child.isValid() || !mSceneGraph) {
            return;
        }

        mSceneGraph->addChild(mId, child.getId());
    }

    void Entity::removeChild(Entity child) {
        if (!isValid() || !child.isValid() || !mSceneGraph) {
            return;
        }

        mSceneGraph->removeChild(mId, child.getId());
    }

    bool Entity::isAncestorOf(Entity other) const {
        if (!isValid() || !other.isValid() || !mSceneGraph) {
            return false;
        }

        return mSceneGraph->isAncestor(mId, other.getId());
    }

    bool Entity::isDescendantOf(Entity other) const {
        if (!isValid() || !other.isValid() || !mSceneGraph) {
            return false;
        }

        return mSceneGraph->isDescendant(mId, other.getId());
    }

    bool Entity::isRoot() const {
        if (!isValid() || !mSceneGraph) {
            return false;
        }

        return mSceneGraph->isRoot(mId);
    }

    bool Entity::isActive() const {
        if (!isValid() || !mSceneGraph) {
            return false;
        }

        return mSceneGraph->isActive(mId);
    }

    Scene* Entity::getScene() const {
        return mScene;
    }

    SceneGraph* Entity::getSceneGraph() const {
        return mSceneGraph;
    }
}
#include "Entity.h"
#include "Scene.h"
#include "SceneGraph.h"
#include <iostream>
#include <stdexcept>
#include <glm/gtx/quaternion.hpp>

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
            mSceneGraph->mRegistry.valid(mHandle);
    }

    entt::registry& Entity::getRegistry() const {
        return mSceneGraph->mRegistry;
    }

    void Entity::setName(const std::string& name) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setNodeName(name);
        }
    }

    std::string Entity::getName() const {
        if (!isValid() || !mSceneGraph) {
            return "";
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            return node->getNodeName();
        }
        return "";
    }

    std::string Entity::getId() const {
        if (!isValid() || !mSceneGraph) {
            return "";
        }
        for (const auto& pair : mSceneGraph->mNodes) {
            if (pair.second->getEntity().mHandle == mHandle) {
                return pair.first;
            }
        }
        return "";
    }

    // Position controls
    void Entity::setPosition(const glm::vec3& position) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setPosition(position);
        }
    }

    void Entity::setPositionX(float x) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 pos = node->getPosition();
            pos.x = x;
            node->setPosition(pos);
        }
    }

    void Entity::setPositionY(float y) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 pos = node->getPosition();
            pos.y = y;
            node->setPosition(pos);
        }
    }

    void Entity::setPositionZ(float z) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 pos = node->getPosition();
            pos.z = z;
            node->setPosition(pos);
        }
    }

    void Entity::move(const glm::vec3& offset) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setPosition(node->getPosition() + offset);
        }
    }

    void Entity::moveUp(float distance) {
        move(glm::vec3(0.0f, distance, 0.0f));
    }

    void Entity::moveDown(float distance) {
        move(glm::vec3(0.0f, -distance, 0.0f));
    }

    void Entity::moveLeft(float distance) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::quat rotation = node->getRotation();
            glm::vec3 left = rotation * glm::vec3(-1.0f, 0.0f, 0.0f);
            node->setPosition(node->getPosition() + left * distance);
        }
    }

    void Entity::moveRight(float distance) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::quat rotation = node->getRotation();
            glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            node->setPosition(node->getPosition() + right * distance);
        }
    }

    void Entity::moveForward(float distance) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::quat rotation = node->getRotation();
            glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            node->setPosition(node->getPosition() + forward * distance);
        }
    }

    void Entity::moveBackward(float distance) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::quat rotation = node->getRotation();
            glm::vec3 backward = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
            node->setPosition(node->getPosition() + backward * distance);
        }
    }

    glm::vec3 Entity::getPosition() const {
        if (!isValid() || !mSceneGraph) {
            return glm::vec3(0.0f);
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            return node->getPosition();
        }
        return glm::vec3(0.0f);
    }


    // Rotation controls
    void Entity::setRotation(const glm::quat& rotation) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setRotation(rotation);
        }
    }

    void Entity::setRotationEuler(const glm::vec3& rotation) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setEulerRotation(rotation);
        }
    }

    void Entity::setRotationX(float angleDegrees) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 euler = node->getEulerRotation();
            euler.x = angleDegrees;
            node->setEulerRotation(euler);
        }
    }

    void Entity::setRotationY(float angleDegrees) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 euler = node->getEulerRotation();
            euler.y = angleDegrees;
            node->setEulerRotation(euler);
        }
    }

    void Entity::setRotationZ(float angleDegrees) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 euler = node->getEulerRotation();
            euler.z = angleDegrees;
            node->setEulerRotation(euler);
        }
    }


    void Entity::rotateEuler(const glm::vec3& anglesDegrees) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 currentEuler = node->getEulerRotation();
            node->setEulerRotation(currentEuler + anglesDegrees);
        }
    }


    void Entity::rotateQuaternion(float angleDegrees, glm::vec3 axis) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setQuaternionRotation(angleDegrees, axis);
        }
    }


    void Entity::rotateX(float angleDegrees) {
        float angleRad = glm::radians(angleDegrees);
        glm::vec3 axis(1.0f,0.0f,0.0f);
        rotateQuaternion(angleDegrees, axis);
        //rotateEuler(glm::vec3(angleDegrees, 0.0f, 0.0f));
    }

    void Entity::rotateY(float angleDegrees) {
        float angleRad = glm::radians(angleDegrees);
        glm::vec3 axis(0.0f,1.0f,0.0f);
        rotateQuaternion(angleDegrees, axis);
    //    rotateEuler(glm::vec3(0.0f, angleDegrees, 0.0f));
    }

    void Entity::rotateZ(float angleDegrees) {
        float angleRad = glm::radians(angleDegrees);
        glm::vec3 axis(0.0f,0.0f,1.0f);
        rotateQuaternion(angleDegrees, axis);
        //rotateEuler(glm::vec3(0.0f, 0.0f, angleDegrees));
    }

    glm::quat Entity::getRotation() const {
        if (!isValid() || !mSceneGraph) {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            return node->getRotation();
        }
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Entity::getEulerRotation() const {
        if (!isValid() || !mSceneGraph) {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            return node->getEulerRotation();
        }
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }


    // Scale controls
    void Entity::setScale(const glm::vec3& scale) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setScale(scale);
        }
    }

    void Entity::setScaleX(float x) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 scl = node->getScale();
            scl.x = x;
            node->setScale(scl);
        }
    }

    void Entity::setScaleY(float y) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 scl = node->getScale();
            scl.y = y;
            node->setScale(scl);
        }
    }

    void Entity::setScaleZ(float z) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            glm::vec3 scl = node->getScale();
            scl.z = z;
            node->setScale(scl);
        }
    }

    void Entity::scale(const glm::vec3& factor) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            node->setScale(node->getScale() * factor);
        }
    }

    void Entity::scaleX(float x) {
        scale(glm::vec3(x, 1.0f, 1.0f));
    }

    void Entity::scaleY(float y) {
        scale(glm::vec3(1.0f, y, 1.0f));
    }

    void Entity::scaleZ(float z) {
        scale(glm::vec3(1.0f, 1.0f, z));
    }

    glm::vec3 Entity::getScale() const {
        if (!isValid() || !mSceneGraph) {
            return glm::vec3(1.0f);
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            return node->getScale();
        }
        return glm::vec3(1.0f);
    }


    glm::mat4 Entity::getLocalMatrix() const {
        if (!isValid() || !mSceneGraph) {
            return glm::mat4(1.0f);
        }
        auto node = mSceneGraph->getNode(getId());
        if (node) {
            return node->getLocalMatrix();
        }
        return glm::mat4(1.0f);
    }

    glm::mat4 Entity::getWorldMatrix() const {
        if (!isValid() || !mSceneGraph) {
            return glm::mat4(1.0f);
        }
        auto node = mSceneGraph->getNode(getId());
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
            mSceneGraph->setParent(getId(), parent.getId());
        }
    }

    Entity Entity::getParent() const {
        if (!isValid() || !mSceneGraph) {
            return Entity();
        }
        return mSceneGraph->getParent(getId());
    }

    std::vector<Entity> Entity::getChildren() const {
        if (!isValid() || !mSceneGraph) {
            return std::vector<Entity>();
        }
        return mSceneGraph->getChildren(getId());
    }

    void Entity::addChild(Entity child) {
        if (!isValid() || !child.isValid() || !mSceneGraph) {
            return;
        }
        mSceneGraph->addChild(getId(), child.getId());
    }

    void Entity::removeChild(Entity child) {
        if (!isValid() || !child.isValid() || !mSceneGraph) {
            return;
        }
        mSceneGraph->removeChild(getId(), child.getId());
    }

    bool Entity::isAncestorOf(Entity other) const {
        if (!isValid() || !other.isValid() || !mSceneGraph) {
            return false;
        }
        return mSceneGraph->isAncestor(getId(), other.getId());
    }

    bool Entity::isDescendantOf(Entity other) const {
        if (!isValid() || !other.isValid() || !mSceneGraph) {
            return false;
        }
        return mSceneGraph->isDescendant(getId(), other.getId());
    }

    bool Entity::isRoot() const {
        if (!isValid() || !mSceneGraph) {
            return false;
        }
        return mSceneGraph->isRoot(getId());
    }

    bool Entity::isActive() const {
        if (!isValid() || !mSceneGraph) {
            return false;
        }
        return mSceneGraph->isActive(getId());
    }

    Scene* Entity::getScene() const {
        return mScene;
    }

    SceneGraph* Entity::getSceneGraph() const {
        return mSceneGraph;
    }
}

namespace std {
    template<>
    struct hash<StrikeEngine::Entity> {
        std::size_t operator()(const StrikeEngine::Entity& e) const noexcept {
            return std::hash<entt::entity>()(e.mHandle);
        }
    };
}
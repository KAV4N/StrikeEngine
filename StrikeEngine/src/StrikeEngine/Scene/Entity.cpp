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

    // Position controls
    void Entity::setPosition(const glm::vec3& position) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setPosition(position);
        }
    }

    void Entity::setPositionX(float x) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setPosition(node->getPosition() + offset);
        }
    }

    void Entity::moveX(float x) {
        move(glm::vec3(x, 0.0f, 0.0f));
    }

    void Entity::moveY(float y) {
        move(glm::vec3(0.0f, y, 0.0f));
    }

    void Entity::moveZ(float z) {
        move(glm::vec3(0.0f, 0.0f, z));
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

    float Entity::getPositionX() const {
        return getPosition().x;
    }

    float Entity::getPositionY() const {
        return getPosition().y;
    }

    float Entity::getPositionZ() const {
        return getPosition().z;
    }

    // Rotation controls
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

    void Entity::setRotationX(float angleDegrees) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            glm::vec3 euler = node->getEulerRotation();
            euler.z = angleDegrees;
            node->setEulerRotation(euler);
        }
    }

    void Entity::rotate(const glm::quat& rotation) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setRotation(glm::normalize(rotation * node->getRotation()));
        }
    }

    void Entity::rotateEuler(const glm::vec3& anglesDegrees) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            glm::vec3 currentEuler = node->getEulerRotation();
            node->setEulerRotation(currentEuler + anglesDegrees);
        }
    }

    void Entity::rotateX(float angleDegrees) {
        rotateEuler(glm::vec3(angleDegrees, 0.0f, 0.0f));
    }

    void Entity::rotateY(float angleDegrees) {
        rotateEuler(glm::vec3(0.0f, angleDegrees, 0.0f));
    }

    void Entity::rotateZ(float angleDegrees) {
        rotateEuler(glm::vec3(0.0f, 0.0f, angleDegrees));
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
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            return node->getEulerRotation();
        }
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    float Entity::getRotationX() const {
        return getEulerRotation().x;
    }

    float Entity::getRotationY() const {
        return getEulerRotation().y;
    }

    float Entity::getRotationZ() const {
        return getEulerRotation().z;
    }

    // Scale controls
    void Entity::setScale(const glm::vec3& scale) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setScale(scale);
        }
    }

    void Entity::setScaleX(float x) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
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
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            glm::vec3 scl = node->getScale();
            scl.z = z;
            node->setScale(scl);
        }
    }

    void Entity::scaleBy(const glm::vec3& factor) {
        if (!isValid() || !mSceneGraph) {
            return;
        }
        auto node = mSceneGraph->getNode(mId);
        if (node) {
            node->setScale(node->getScale() * factor);
        }
    }

    void Entity::scaleByX(float x) {
        scaleBy(glm::vec3(x, 1.0f, 1.0f));
    }

    void Entity::scaleByY(float y) {
        scaleBy(glm::vec3(1.0f, y, 1.0f));
    }

    void Entity::scaleByZ(float z) {
        scaleBy(glm::vec3(1.0f, 1.0f, z));
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

    float Entity::getScaleX() const {
        return getScale().x;
    }

    float Entity::getScaleY() const {
        return getScale().y;
    }

    float Entity::getScaleZ() const {
        return getScale().z;
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
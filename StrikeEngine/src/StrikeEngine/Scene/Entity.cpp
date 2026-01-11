#include "strikepch.h"
#include "Entity.h"
#include "GraphNode.h"
#include "Scene.h"
#include "World.h"
#include "Systems/PhysicsSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


namespace StrikeEngine {

    Entity::Entity(entt::entity handle, Scene* scene)
        : mHandle(handle), mScene(scene) {}

    bool Entity::isValid() const {
        return mScene && mScene->getRegistry().valid(mHandle);
    }

    bool Entity::isActive() const {
        if (!isValid()) return false;
        return mScene->getGraphNode(mHandle)->isActive();; 
    }

    void Entity::setActive(bool active) {
        if (!isValid()) return;
        mScene->getGraphNode(mHandle)->setActive(active);
    }

    // Tag operations now use GraphNode directly
    void Entity::setTag(const std::string& tag) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->setTag(tag);
        }
    }

    const std::string& Entity::getTag() const {
        static const std::string empty = "";
        if (!isValid()) return empty;
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getTag() : empty;
    }

    // Transform operations
    void Entity::setPosition(const glm::vec3& position) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->setLocalPosition(position);
        }
    }

    void Entity::move(const glm::vec3& offset) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->move(offset);
        }
    }

    void Entity::setRotation(const glm::quat& rotation) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->setLocalRotation(rotation);
        }
    }

    void Entity::setRotationEuler(const glm::vec3& rotation) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->setLocalRotationEuler(rotation);
        }
    }

    void Entity::rotateEuler(const glm::vec3& anglesDegrees) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->rotateEuler(anglesDegrees);
        }
    }

    void Entity::rotateX(float angleDegrees) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->rotateQuaternion(angleDegrees, glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }

    void Entity::rotateY(float angleDegrees) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->rotateQuaternion(angleDegrees, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    void Entity::rotateZ(float angleDegrees) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->rotateQuaternion(angleDegrees, glm::vec3(0.0f, 0.0f, 1.0f));
        }
    }

    void Entity::setScale(const glm::vec3& scale) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->setLocalScale(scale);
        }
    }

    void Entity::scaleBy(const glm::vec3& factor) {
        if (!isValid()) return;
        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            node->scale(factor);
        }
    }

    glm::vec3 Entity::getPosition() const {
        if (!isValid()) return glm::vec3(0.0f);
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getLocalPosition() : glm::vec3(0.0f);
    }

    glm::quat Entity::getRotation() const {
        if (!isValid()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getLocalRotation() : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Entity::getEulerRotation() const {
        if (!isValid()) return glm::vec3(0.0f);
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getLocalRotationEuler() : glm::vec3(0.0f);
    }

    glm::vec3 Entity::getScale() const {
        if (!isValid()) return glm::vec3(1.0f);
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getLocalScale() : glm::vec3(1.0f);
    }

    glm::mat4 Entity::getLocalMatrix() const {
        if (!isValid()) return glm::mat4(1.0f);
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getLocalMatrix() : glm::mat4(1.0f);
    }

    glm::mat4 Entity::getWorldMatrix() const {
        if (!isValid()) return glm::mat4(1.0f);
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getWorldMatrix() : glm::mat4(1.0f);
    }

    // Hierarchy operations
    bool Entity::isRoot() const {
        if (!isValid()) return false;
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->isRoot() : false;
    }

    Entity Entity::getParent() const {
        if (!isValid()) return Entity();
        auto node = mScene->getGraphNode(mHandle);
        if (node && node->getParent()) {
            return Entity(node->getParent()->getEntityId(), mScene);
        }
        return Entity();
    }

    void Entity::setParent(Entity parent) {
        if (!isValid() || !parent.isValid()) {
            return;
        }
        mScene->setParent(*this, parent);
    }

    void Entity::addChild(Entity child) {
        if (!isValid() || !child.isValid()) {
            return;
        }
        mScene->addChild(*this, child);
    }

    std::vector<Entity> Entity::getChildren() const {
        std::vector<Entity> result;
        if (!isValid()) return result;

        auto node = mScene->getGraphNode(mHandle);
        if (node) {
            for (const auto& childNode : node->getChildren()) {
                result.emplace_back(childNode->getEntityId(), mScene);
            }
        }
        return result;
    }

    size_t Entity::getChildCount() const {
        if (!isValid()) return 0;
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->getChildCount() : 0;
    }

    bool Entity::hasChildren() const {
        if (!isValid()) return false;
        auto node = mScene->getGraphNode(mHandle);
        return node ? node->hasChildren() : false;
    }

    std::vector<Entity> Entity::getCollidingEntities() const {
        if (!isValid()) {
            return std::vector<Entity>();
        }

        PhysicsSystem* physicsSystem = World::get().mPhysicsSystem.get();
        if (!physicsSystem) {
            return std::vector<Entity>();
        }

        return physicsSystem->getCollidingEntities(*this);
    }

    bool Entity::isCollidingWith(const Entity& other) const {
        if (!isValid() || !other.isValid()) {
            return false;
        }

        PhysicsSystem* physicsSystem = World::get().mPhysicsSystem.get();
        if (!physicsSystem) {
            return false;
        }

        return physicsSystem->isColliding(*this, other);
    }

    void Entity::destroy() const {
        if (!isValid()) {
            return;
        }
        mScene->destroy(mHandle);
    }

} // namespace StrikeEngine
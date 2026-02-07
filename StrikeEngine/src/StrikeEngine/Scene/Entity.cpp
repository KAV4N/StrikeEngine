#include "strikepch.h"
#include "Entity.h"
#include "GraphNode.h"
#include "Scene.h"
#include "World.h"
#include "Systems/PhysicsSystem.h"


namespace StrikeEngine {

    Entity::Entity(entt::entity handle, Scene* scene)
        : mHandle(handle), mScene(scene) {}

    bool Entity::isValid() const {
        return mScene && mScene->getRegistry().valid(mHandle);
    }

    bool Entity::isActive() const {
        STRIKE_ASSERT(isValid(), "Entity::isActive called on invalid entity");
        return mScene->getGraphNode(mHandle)->isActive();
    }

    void Entity::setActive(bool active) {
        STRIKE_ASSERT(isValid(), "Entity::setActive called on invalid entity");
        mScene->getGraphNode(mHandle)->setActive(active);
    }

    // ===============================
    // Tag
    // ===============================
    void Entity::setTag(const std::string& tag) {
        STRIKE_ASSERT(isValid(), "Entity::setTag called on invalid entity");
        mScene->getGraphNode(mHandle)->setTag(tag);
    }

    const std::string& Entity::getTag() const {
        STRIKE_ASSERT(isValid(), "Entity::getTag called on invalid entity");
        return mScene->getGraphNode(mHandle)->getTag();
    }

    // ===============================
    // Local transform
    // ===============================
    const glm::vec3& Entity::getPosition() const {
        STRIKE_ASSERT(isValid(), "Entity::getPosition called on invalid entity");
        return mScene->getGraphNode(mHandle)->getPosition();
    }

    void Entity::setPosition(const glm::vec3& position) {
        STRIKE_ASSERT(isValid(), "Entity::setPosition called on invalid entity");
        mScene->getGraphNode(mHandle)->setPosition(position);
    }

    void Entity::move(const glm::vec3& delta) {
        STRIKE_ASSERT(isValid(), "Entity::move called on invalid entity");
        mScene->getGraphNode(mHandle)->move(delta);
    }

    const glm::quat& Entity::getRotation() const {
        STRIKE_ASSERT(isValid(), "Entity::getRotation called on invalid entity");
        return mScene->getGraphNode(mHandle)->getRotation();
    }

    void Entity::setRotation(const glm::quat& rotation) {
        STRIKE_ASSERT(isValid(), "Entity::setRotation called on invalid entity");
        mScene->getGraphNode(mHandle)->setRotation(rotation);
    }

    glm::vec3 Entity::getEulerAngles() const {
        STRIKE_ASSERT(isValid(), "Entity::getEulerAngles called on invalid entity");
        return mScene->getGraphNode(mHandle)->getEulerAngles();
    }

    void Entity::setEulerAngles(const glm::vec3& angles) {
        STRIKE_ASSERT(isValid(), "Entity::setEulerAngles called on invalid entity");
        mScene->getGraphNode(mHandle)->setEulerAngles(angles);
    }

    const glm::vec3& Entity::getScale() const {
        STRIKE_ASSERT(isValid(), "Entity::getScale called on invalid entity");
        return mScene->getGraphNode(mHandle)->getScale();
    }

    void Entity::setScale(const glm::vec3& scale) {
        STRIKE_ASSERT(isValid(), "Entity::setScale called on invalid entity");
        mScene->getGraphNode(mHandle)->setScale(scale);
    }

    void Entity::rotate(const glm::vec3& angles) {
        STRIKE_ASSERT(isValid(), "Entity::rotate called on invalid entity");
        mScene->getGraphNode(mHandle)->rotate(angles);
    }

    void Entity::rotateQuaternion(float angleDegrees, const glm::vec3& axis) {
        STRIKE_ASSERT(isValid(), "Entity::rotateQuaternion called on invalid entity");
        mScene->getGraphNode(mHandle)->rotateQuaternion(angleDegrees, axis);
    }

    // ===============================
    // World transform
    // ===============================
    glm::vec3 Entity::getWorldPosition() const {
        STRIKE_ASSERT(isValid(), "Entity::getWorldPosition called on invalid entity");
        return mScene->getGraphNode(mHandle)->getWorldPosition();
    }

    void Entity::setWorldPosition(const glm::vec3& position) {
        STRIKE_ASSERT(isValid(), "Entity::setWorldPosition called on invalid entity");
        mScene->getGraphNode(mHandle)->setWorldPosition(position);
    }

    glm::quat Entity::getWorldRotation() const {
        STRIKE_ASSERT(isValid(), "Entity::getWorldRotation called on invalid entity");
        return mScene->getGraphNode(mHandle)->getWorldRotation();
    }

    void Entity::setWorldRotation(const glm::quat& rotation) {
        STRIKE_ASSERT(isValid(), "Entity::setWorldRotation called on invalid entity");
        mScene->getGraphNode(mHandle)->setWorldRotation(rotation);
    }

    glm::vec3 Entity::getWorldEulerAngles() const {
        STRIKE_ASSERT(isValid(), "Entity::getWorldEulerAngles called on invalid entity");
        return mScene->getGraphNode(mHandle)->getWorldEulerAngles();
    }

    void Entity::setWorldEulerAngles(const glm::vec3& angles) {
        STRIKE_ASSERT(isValid(), "Entity::setWorldEulerAngles called on invalid entity");
        mScene->getGraphNode(mHandle)->setWorldEulerAngles(angles);
    }

    glm::vec3 Entity::getWorldScale() const {
        STRIKE_ASSERT(isValid(), "Entity::getWorldScale called on invalid entity");
        return mScene->getGraphNode(mHandle)->getWorldScale();
    }

    // ===============================
    // Direction vectors
    // ===============================
    glm::vec3 Entity::getForward() const {
        STRIKE_ASSERT(isValid(), "Entity::getForward called on invalid entity");
        return mScene->getGraphNode(mHandle)->getForward();
    }

    glm::vec3 Entity::getRight() const {
        STRIKE_ASSERT(isValid(), "Entity::getRight called on invalid entity");
        return mScene->getGraphNode(mHandle)->getRight();
    }

    glm::vec3 Entity::getUp() const {
        STRIKE_ASSERT(isValid(), "Entity::getUp called on invalid entity");
        return mScene->getGraphNode(mHandle)->getUp();
    }

    // ===============================
    // Matrices
    // ===============================
    const glm::mat4& Entity::getLocalMatrix() const {
        STRIKE_ASSERT(isValid(), "Entity::getLocalMatrix called on invalid entity");
        return mScene->getGraphNode(mHandle)->getLocalMatrix();
    }

    const glm::mat4& Entity::getWorldMatrix() const {
        STRIKE_ASSERT(isValid(), "Entity::getWorldMatrix called on invalid entity");
        return mScene->getGraphNode(mHandle)->getWorldMatrix();
    }

    // ===============================
    // Hierarchy
    // ===============================
    bool Entity::isRoot() const {
        STRIKE_ASSERT(isValid(), "Entity::isRoot called on invalid entity");
        return mScene->getGraphNode(mHandle)->isRoot();
    }

    Entity Entity::getParent() const {
        STRIKE_ASSERT(isValid(), "Entity::getParent called on invalid entity");
        auto* parent = mScene->getGraphNode(mHandle)->getParent();
        return parent ? Entity(parent->getEntityId(), mScene) : Entity();
    }

    void Entity::setParent(Entity parent) {
        STRIKE_ASSERT(isValid(), "Entity::setParent called on invalid entity");
        STRIKE_ASSERT(parent.isValid(), "Entity::setParent called with invalid parent");
        STRIKE_ASSERT(parent != *this, "Entity cannot be parent of itself");

        mScene->setParent(*this, parent);
    }

    void Entity::removeFromParent() {
        STRIKE_ASSERT(isValid(), "Entity::removeFromParent called on invalid entity");
        mScene->getGraphNode(mHandle)->removeFromParent();
    }

    void Entity::addChild(Entity child) {
        STRIKE_ASSERT(isValid(), "Entity::addChild called on invalid entity");
        STRIKE_ASSERT(child.isValid(), "Entity::addChild called with invalid child");
        STRIKE_ASSERT(child != *this, "Entity cannot be child of itself");

        mScene->addChild(*this, child);
    }

    std::vector<Entity> Entity::getChildren() const {
        STRIKE_ASSERT(isValid(), "Entity::getChildren called on invalid entity");

        std::vector<Entity> result;
        for (auto& child : mScene->getGraphNode(mHandle)->getChildren()) {
            result.emplace_back(child->getEntityId(), mScene);
        }
        return result;
    }

    // ===============================
    // Physics
    // ===============================
    std::vector<Entity> Entity::getCollidingEntities() const {
        STRIKE_ASSERT(isValid(), "Entity::getCollidingEntities called on invalid entity");

        auto* physics = World::get().mPhysicsSystem.get();
        STRIKE_ASSERT(physics, "PhysicsSystem not initialized");

        return physics->getCollidingEntities(*this);
    }

    bool Entity::isCollidingWith(const Entity& other) const {
        STRIKE_ASSERT(isValid(), "Entity::isCollidingWith called on invalid entity");
        STRIKE_ASSERT(other.isValid(), "Entity::isCollidingWith called with invalid entity");

        auto* physics = World::get().mPhysicsSystem.get();
        STRIKE_ASSERT(physics, "PhysicsSystem not initialized");

        return physics->isColliding(*this, other);
    }

    // ===============================
    // Lifecycle
    // ===============================
    void Entity::destroy() const {
        STRIKE_ASSERT(isValid(), "Entity::destroy called on invalid entity");
        mScene->destroy(mHandle);
    }

} 

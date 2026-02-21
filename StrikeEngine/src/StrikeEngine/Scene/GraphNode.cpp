#include "strikepch.h"
#include "GraphNode.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Strike {

    GraphNode::GraphNode(entt::entity entityId)
        : mEntityId(entityId) {}

    // ===============================
    // Local Transform
    // ===============================
    void GraphNode::setPosition(const glm::vec3& position) {
        mPosition = position;
        markDirty();
    }

    void GraphNode::move(const glm::vec3& delta)
    {
        // Local basis vectors (object space)
        const glm::vec3 localRight   = glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 localUp      = glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::vec3 localForward = glm::vec3(0.0f, 0.0f, 1.0f);

        // Rotate basis vectors into world space using local rotation
        const glm::vec3 worldRight   = mRotation * localRight;
        const glm::vec3 worldUp      = mRotation * localUp;
        const glm::vec3 worldForward = mRotation * localForward;

        // Compose movement in world space
        glm::vec3 worldDelta =
            worldRight   * delta.x +
            worldUp      * delta.y +
            worldForward * delta.z;

        // Apply to local position
        mPosition += worldDelta;

        markDirty();
    }



    void GraphNode::setRotation(const glm::quat& rotation) {
        mRotation = glm::normalize(rotation);
        markDirty();
    }

    void GraphNode::rotate(const glm::vec3& angles) {
        rotateQuaternion(angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
        rotateQuaternion(angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
        rotateQuaternion(angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
        markDirty();
    }

    void GraphNode::rotateQuaternion(float angleDegrees, const glm::vec3& axis)
    {
        glm::vec3 normAxis = glm::normalize(axis);

        glm::quat angleQuat = glm::angleAxis(
            glm::radians(angleDegrees),
            normAxis
        );

        if (normAxis.y == 1.0f)
            mRotation = angleQuat * mRotation;
        else
            mRotation = mRotation * angleQuat;

        mRotation = glm::normalize(mRotation);  
        
        markDirty();
    }

    glm::vec3 GraphNode::getEulerAngles() const {
        return glm::degrees(glm::eulerAngles(mRotation));
    }

    void GraphNode::setEulerAngles(const glm::vec3& angles) {
        mRotation = glm::normalize(glm::quat(glm::radians(angles)));
        markDirty();
    }

    void GraphNode::setScale(const glm::vec3& scale) {
        mScale = scale;
        markDirty();
    }

    // ===============================
    // World Transform
    // ===============================
    glm::vec3 GraphNode::getWorldPosition() const {
        return glm::vec3(getWorldMatrix()[3]);
    }

    void GraphNode::setWorldPosition(const glm::vec3& position) {
        if (auto parent = mParent.lock()) {
            glm::mat4 invParent = glm::inverse(parent->getWorldMatrix());
            mPosition = glm::vec3(invParent * glm::vec4(position, 1.0f));
        } else {
            mPosition = position;
        }
        markDirty();
    }

    glm::quat GraphNode::getWorldRotation() const {
        if (auto parent = mParent.lock()) {
            return parent->getWorldRotation() * mRotation;
        }
        return mRotation;
    }

    void GraphNode::setWorldRotation(const glm::quat& rotation) {
        if (auto parent = mParent.lock()) {
            mRotation = glm::inverse(parent->getWorldRotation()) * rotation;
        } else {
            mRotation = rotation;
        }
        mRotation = glm::normalize(mRotation);
        markDirty();
    }

    glm::vec3 GraphNode::getWorldEulerAngles() const {
        return glm::degrees(glm::eulerAngles(getWorldRotation()));
    }

    void GraphNode::setWorldEulerAngles(const glm::vec3& angles) {
        setWorldRotation(glm::quat(glm::radians(angles)));
    }

    glm::vec3 GraphNode::getWorldScale() const {
        glm::vec3 result = mScale;
        if (auto parent = mParent.lock()) {
            result *= parent->getWorldScale();
        }
        return result;
    }

    // ===============================
    // Direction vectors
    // ===============================
    glm::vec3 GraphNode::getForward() const {
        return glm::normalize(glm::vec3(-getWorldMatrix()[2]));
    }

    glm::vec3 GraphNode::getRight() const {
        return glm::normalize(glm::vec3(getWorldMatrix()[0]));
    }

    glm::vec3 GraphNode::getUp() const {
        return glm::normalize(glm::vec3(getWorldMatrix()[1]));
    }

    // ===============================
    // Matrices
    // ===============================
    void GraphNode::updateLocalMatrix() const {
        if (!mIsDirty) return;
        glm::mat4 T = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 R = glm::mat4_cast(mRotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), mScale);
        mLocalMatrix = T * R * S;
    
    }

    void GraphNode::updateWorldMatrix() const {
        if (!mIsDirty) return;

        updateLocalMatrix();

        if (auto parent = mParent.lock()) {
            mWorldMatrix = parent->getWorldMatrix() * mLocalMatrix;
        } else {
            mWorldMatrix = mLocalMatrix;
        }



    }

    const glm::mat4& GraphNode::getLocalMatrix() const {
        updateLocalMatrix();
        return mLocalMatrix;
    }

    const glm::mat4& GraphNode::getWorldMatrix() const {
        updateWorldMatrix();
        return mWorldMatrix;
    }

    // ===============================
    // Hierarchy
    // ===============================
    GraphNode* GraphNode::getParent() const {
        auto p = mParent.lock();
        return p ? p.get() : nullptr;
    }

    void GraphNode::setParent(std::shared_ptr<GraphNode> parent) {
        if (auto oldParent = mParent.lock()) {
            oldParent->removeChild(this);
        }

        mParent = parent;

        if (parent) {
            parent->mChildren.push_back(shared_from_this());
        }

        markDirty();
    }

    void GraphNode::removeFromParent() {
        if (auto oldParent = mParent.lock()) {
            oldParent->removeChild(this);
            mParent.reset();
            markDirty();
        }
    }

    void GraphNode::addChild(std::shared_ptr<GraphNode> child) {
        child->setParent(shared_from_this());
    }

    void GraphNode::removeChild(GraphNode* child) {
        auto it = std::find_if(
            mChildren.begin(), mChildren.end(),
            [child](const std::shared_ptr<GraphNode>& node) {
                return node.get() == child;
            });

        if (it != mChildren.end()) {
            mChildren.erase(it);
        }
    }

    // ===============================
    // Dirty propagation
    // ===============================
    void GraphNode::markDirty() {
        mIsDirty = true;
        /*
        for (auto& child : mChildren) {
            child->markDirty();
        }
        */  
    }

} 

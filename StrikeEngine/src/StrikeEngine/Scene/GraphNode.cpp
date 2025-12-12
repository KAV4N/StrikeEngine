#include "GraphNode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>

namespace StrikeEngine {

    GraphNode::GraphNode(entt::entity entityId)
        : mEntityId(entityId) {}

    void GraphNode::setLocalPosition(const glm::vec3& position) {
        mLocalPosition = position;
        markDirty();
    }

    void GraphNode::move(const glm::vec3& offset) {
        mLocalPosition += offset;
        markDirty();
    }

    void GraphNode::setLocalRotation(const glm::quat& rotation) {
        mLocalRotation = rotation;
        markDirty();
    }

    void GraphNode::setLocalRotationEuler(const glm::vec3& eulerAngles) {
        glm::vec3 eulerRad = glm::radians(eulerAngles);
        mLocalRotation = glm::normalize(glm::quat(eulerRad));
        markDirty();
    }

    void GraphNode::rotateEuler(const glm::vec3& anglesDegrees) {
        rotateQuaternion(anglesDegrees.x, glm::vec3(1.0f, 0.0f, 0.0f));
        rotateQuaternion(anglesDegrees.y, glm::vec3(0.0f, 1.0f, 0.0f));
        rotateQuaternion(anglesDegrees.z, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void GraphNode::rotateQuaternion(float angleDegrees, const glm::vec3& axis) {
        glm::quat angleQuat = glm::angleAxis(glm::radians(angleDegrees), axis);
        if (axis.y == 1.0f) {
            mLocalRotation = angleQuat * mLocalRotation;
        } else {
            mLocalRotation = mLocalRotation * angleQuat;
        }
        markDirty();
    }

    glm::vec3 GraphNode::getLocalRotationEuler() const {
        glm::vec3 eulerRad = glm::eulerAngles(mLocalRotation);
        return glm::degrees(eulerRad);
    }

    void GraphNode::setLocalScale(const glm::vec3& scale) {
        mLocalScale = scale;
        markDirty();
    }

    void GraphNode::scale(const glm::vec3& factor) {
        mLocalScale *= factor;
        markDirty();
    }

    void GraphNode::updateLocalMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), mLocalPosition);
        glm::mat4 rotation = glm::mat4_cast(mLocalRotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), mLocalScale);
        mLocalMatrix = translation * rotation * scale;
        mIsDirty = false;
    }

    const glm::mat4& GraphNode::getLocalMatrix() const {
        if (mIsDirty) {
            updateLocalMatrix();
        }
        return mLocalMatrix;
    }

    void GraphNode::markDirty() {
        mIsDirty = true;
        /*
        for (auto& child : mChildren) {
            child->markDirty();
        }
        */
    }

   void GraphNode::setParent(std::shared_ptr<GraphNode> parent) {
        if (auto oldParent = mParent.lock()) {
            oldParent->removeChild(this);
        }

        mParent = parent;
        parent->mChildren.push_back(shared_from_this());
        

        markDirty();
    }


    void GraphNode::removeParent() {
        if (auto parent = mParent.lock()) {
            parent->removeChild(this);
        }
        mParent.reset();
        markDirty();
    }

    void GraphNode::addChild(std::shared_ptr<GraphNode> child) {
        child->setParent(shared_from_this());
    }

    void GraphNode::removeChild(GraphNode* child) {
        auto it = std::find_if(mChildren.begin(), mChildren.end(),
            [child](const std::shared_ptr<GraphNode>& node) {
                return node.get() == child;
            });

        if (it != mChildren.end()) {
            mChildren.erase(it);
        }
    }

} // namespace StrikeEngine
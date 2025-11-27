#include "TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace StrikeEngine {

    void TransformComponent::deserialize(const pugi::xml_node& node) {
        if (auto posNode = node.child("position")) {
            mPosition.x = posNode.attribute("x").as_float(0.0f);
            mPosition.y = posNode.attribute("y").as_float(0.0f);
            mPosition.z = posNode.attribute("z").as_float(0.0f);
        }
        if (auto rotNode = node.child("rotation")) {
            glm::vec3 euler;
            euler.x = rotNode.attribute("x").as_float(0.0f);
            euler.y = rotNode.attribute("y").as_float(0.0f);
            euler.z = rotNode.attribute("z").as_float(0.0f);
            setRotationEuler(euler);
        }
        if (auto scaleNode = node.child("scale")) {
            mScale.x = scaleNode.attribute("x").as_float(1.0f);
            mScale.y = scaleNode.attribute("y").as_float(1.0f);
            mScale.z = scaleNode.attribute("z").as_float(1.0f);
        }
        mIsDirty = true;
    }

    void TransformComponent::serialize(pugi::xml_node& node) const {
        auto posNode = node.append_child("position");
        posNode.append_attribute("x") = mPosition.x;
        posNode.append_attribute("y") = mPosition.y;
        posNode.append_attribute("z") = mPosition.z;

        auto euler = getRotationEuler();
        auto rotNode = node.append_child("rotation");
        rotNode.append_attribute("x") = euler.x;
        rotNode.append_attribute("y") = euler.y;
        rotNode.append_attribute("z") = euler.z;

        auto scaleNode = node.append_child("scale");
        scaleNode.append_attribute("x") = mScale.x;
        scaleNode.append_attribute("y") = mScale.y;
        scaleNode.append_attribute("z") = mScale.z;
    }

    void TransformComponent::setRotationEuler(const glm::vec3& eulerAngles) {
        glm::vec3 eulerRad = glm::radians(eulerAngles);
        mRotation = glm::normalize(glm::quat(eulerRad));
        markDirty();
    }

    void TransformComponent::rotateEuler(const glm::vec3& anglesDegrees) {
        rotateX(anglesDegrees.x);
        rotateY(anglesDegrees.y);
        rotateZ(anglesDegrees.z);
    }

    void TransformComponent::rotateQuaternion(float angleDegrees, glm::vec3 axis) {
        glm::quat angleQuat = glm::angleAxis(glm::radians(angleDegrees), axis);
        if (axis.y == 1.0f) {
            mRotation = angleQuat * mRotation;
        } else {
            mRotation = mRotation * angleQuat;
        }
        markDirty();
    }

    glm::vec3 TransformComponent::getRotationEuler() const {
        glm::vec3 eulerRad = glm::eulerAngles(mRotation);
        return glm::degrees(eulerRad);
    }

    void TransformComponent::updateLocalMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 rotation = glm::mat4_cast(mRotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), mScale);
        mLocalMatrix = translation * rotation * scale;
        mIsDirty = false;
    }
    
    const glm::mat4& TransformComponent::getLocalMatrix() const {
        if (mIsDirty) {
            updateLocalMatrix();
        }
        return mLocalMatrix;
    }

} // namespace StrikeEngine
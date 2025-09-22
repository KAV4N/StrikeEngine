#include "CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <sstream>

namespace StrikeEngine {

    CameraComponent::CameraComponent() {
        updateProjectionMatrix(glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    }

    CameraComponent::CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane)
        : mFOV(fov)
        , mAspectRatio(aspectRatio)
        , mNearPlane(nearPlane)
        , mFarPlane(farPlane) {
        updateProjectionMatrix(glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    }

    void CameraComponent::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        mFOV = fov;
        mAspectRatio = aspectRatio;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
    }

    void CameraComponent::setFOV(float fov) {
        mFOV = std::clamp(fov, 1.0f, 179.0f);
    }

    void CameraComponent::setAspectRatio(float aspectRatio) {
        mAspectRatio = std::max(aspectRatio, 0.01f);
    }

    void CameraComponent::setNearPlane(float nearPlane) {
        mNearPlane = std::max(nearPlane, 0.001f);
    }

    void CameraComponent::setFarPlane(float farPlane) {
        mFarPlane = std::max(farPlane, mNearPlane + 0.001f);
    }

    void CameraComponent::setViewportRect(float x, float y, float width, float height) {
        mViewportRect.x = std::clamp(x, 0.0f, 1.0f);
        mViewportRect.y = std::clamp(y, 0.0f, 1.0f);
        mViewportRect.width = std::clamp(width, 0.0f, 1.0f - mViewportRect.x);
        mViewportRect.height = std::clamp(height, 0.0f, 1.0f - mViewportRect.y);
    }

    glm::mat4 CameraComponent::getProjectionMatrix() {
        return mProjectionMatrix;
    }

    glm::mat4 CameraComponent::getViewMatrix() {
        return mViewMatrix;
    }

    glm::mat4 CameraComponent::getViewProjectionMatrix() {
        return mViewProjectionMatrix;
    }

    void CameraComponent::update(const glm::vec3& position, const glm::quat& rotation) {
        updateViewMatrix(position, rotation);
        updateProjectionMatrix(position, rotation);
        updateViewProjectionMatrix(position, rotation);
        calculateFrustum(position, rotation);

        // Update direction vectors
        mForward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        mUp = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        mRight = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    void CameraComponent::updateProjectionMatrix(const glm::vec3& position, const glm::quat& rotation) {
        mProjectionMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearPlane, mFarPlane);
    }

    void CameraComponent::updateViewMatrix(const glm::vec3& position, const glm::quat& rotation) {
        glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        mViewMatrix = glm::lookAt(position, position + forward, up);
    }

    void CameraComponent::updateViewProjectionMatrix(const glm::vec3& position, const glm::quat& rotation) {
        mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
    }

    void CameraComponent::calculateFrustum(const glm::vec3& position, const glm::quat& rotation) {
        glm::mat4 viewProj = mViewProjectionMatrix;

        // Extract frustum planes from view-projection matrix
        mFrustum.planes[0] = glm::vec4(
            viewProj[0][3] + viewProj[0][0],
            viewProj[1][3] + viewProj[1][0],
            viewProj[2][3] + viewProj[2][0],
            viewProj[3][3] + viewProj[3][0]
        );

        mFrustum.planes[1] = glm::vec4(
            viewProj[0][3] - viewProj[0][0],
            viewProj[1][3] - viewProj[1][0],
            viewProj[2][3] - viewProj[2][0],
            viewProj[3][3] - viewProj[3][0]
        );

        mFrustum.planes[2] = glm::vec4(
            viewProj[0][3] + viewProj[0][1],
            viewProj[1][3] + viewProj[1][1],
            viewProj[2][3] + viewProj[2][1],
            viewProj[3][3] + viewProj[3][1]
        );

        mFrustum.planes[3] = glm::vec4(
            viewProj[0][3] - viewProj[0][1],
            viewProj[1][3] - viewProj[1][1],
            viewProj[2][3] - viewProj[2][1],
            viewProj[3][3] - viewProj[3][1]
        );

        mFrustum.planes[4] = glm::vec4(
            viewProj[0][3] + viewProj[0][2],
            viewProj[1][3] + viewProj[1][2],
            viewProj[2][3] + viewProj[2][2],
            viewProj[3][3] + viewProj[3][2]
        );

        mFrustum.planes[5] = glm::vec4(
            viewProj[0][3] - viewProj[0][2],
            viewProj[1][3] - viewProj[1][2],
            viewProj[2][3] - viewProj[2][2],
            viewProj[3][3] - viewProj[3][2]
        );

        // Normalize frustum planes
        for (int i = 0; i < 6; i++) {
            float length = glm::length(glm::vec3(mFrustum.planes[i]));
            if (length > 0.0f) {
                mFrustum.planes[i] /= length;
            }
        }
    }

    void CameraComponent::deserialize(const pugi::xml_node& node) {
        // Near/Far planes
        if (auto attr = node.attribute("nearPlane")) {
            mNearPlane = attr.as_float();
        }
        if (auto attr = node.attribute("farPlane")) {
            mFarPlane = attr.as_float();
        }

        // FOV & Aspect Ratio
        if (auto attr = node.attribute("fov")) {
            mFOV = attr.as_float();
        }
        if (auto attr = node.attribute("aspectRatio")) {
            mAspectRatio = attr.as_float();
        }

        // Render order
        if (auto attr = node.attribute("renderOrder")) {
            mRenderOrder = attr.as_int();
        }

        // Viewport rect
        if (auto attr = node.attribute("viewportX")) {
            mViewportRect.x = attr.as_float();
        }
        if (auto attr = node.attribute("viewportY")) {
            mViewportRect.y = attr.as_float();
        }
        if (auto attr = node.attribute("viewportWidth")) {
            mViewportRect.width = attr.as_float();
        }
        if (auto attr = node.attribute("viewportHeight")) {
            mViewportRect.height = attr.as_float();
        }
     
        // Final setup
        setViewportRect(mViewportRect.x, mViewportRect.y, mViewportRect.width, mViewportRect.height);
    }

    void CameraComponent::serialize(pugi::xml_node& node) const {
        node.append_attribute("projectionType") = "perspective";
        node.append_attribute("nearPlane") = mNearPlane;
        node.append_attribute("farPlane") = mFarPlane;
        node.append_attribute("fov") = mFOV;
        node.append_attribute("aspectRatio") = mAspectRatio;

        if (mRenderOrder != 0) {
            node.append_attribute("renderOrder") = mRenderOrder;
        }

        node.append_attribute("viewportX") = mViewportRect.x;
        node.append_attribute("viewportY") = mViewportRect.y;
        node.append_attribute("viewportWidth") = mViewportRect.width;
        node.append_attribute("viewportHeight") = mViewportRect.height;
    }

}
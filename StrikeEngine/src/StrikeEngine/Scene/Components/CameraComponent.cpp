#include "strikepch.h"
#include "CameraComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>



namespace StrikeEngine {

    REGISTER_COMPONENT(CameraComponent);

    CameraComponent::CameraComponent() {
        
    }

    CameraComponent::CameraComponent(float fov, float nearPlane, float farPlane)
        : mFOV(fov)
        , mNearPlane(nearPlane)
        , mFarPlane(farPlane) {
    }

    void CameraComponent::setPerspective(float fov, float nearPlane, float farPlane) {
        mFOV = fov;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
    }

    void CameraComponent::setFOV(float fov) {
        mFOV = std::clamp(fov, 1.0f, 179.0f);
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

    glm::mat4 CameraComponent::getProjectionMatrix() const {
        return mProjectionMatrix;
    }

    glm::mat4 CameraComponent::getViewMatrix() const {
        return mViewMatrix;
    }

    glm::mat4 CameraComponent::getViewProjectionMatrix() const {
        return mViewProjectionMatrix;
    }

    void CameraComponent::update(const glm::mat4& worldMatrix, uint32_t width, uint32_t height) {
        updateViewMatrix(worldMatrix);
        updateProjectionMatrix(width, height);
        updateViewProjectionMatrix();
        calculateFrustum();

        glm::quat rotation = glm::toQuat(worldMatrix);
        mForward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        mUp = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        mRight = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    void CameraComponent::updateProjectionMatrix(uint32_t width, uint32_t height) {
        // Calculate aspect ratio based on viewport size
        float viewportWidth = mViewportRect.width * width;
        float viewportHeight = mViewportRect.height * height;
        float aspectRatio = (viewportHeight > 0.0f) ? (viewportWidth / viewportHeight) : 1.0f;
        mProjectionMatrix = glm::perspective(glm::radians(mFOV), aspectRatio, mNearPlane, mFarPlane);

    }

    void CameraComponent::updateViewMatrix(const glm::mat4& worldMatrix) {
        mViewMatrix = glm::inverse(worldMatrix);
    }

    void CameraComponent::updateViewProjectionMatrix() {
        mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
    }

    void CameraComponent::calculateFrustum() {
        glm::mat4 viewProj = mViewProjectionMatrix;

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

        // FOV
        if (auto attr = node.attribute("fov")) {
            mFOV = attr.as_float();
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

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }


        // Final setup
        setViewportRect(mViewportRect.x, mViewportRect.y, mViewportRect.width, mViewportRect.height);
    }

    void CameraComponent::serialize(pugi::xml_node& node) const {
        node.append_attribute("projectionType") = "perspective";
        node.append_attribute("nearPlane") = mNearPlane;
        node.append_attribute("farPlane") = mFarPlane;
        node.append_attribute("fov") = mFOV;

        if (mRenderOrder != 0) {
            node.append_attribute("renderOrder") = mRenderOrder;
        }

        node.append_attribute("viewportX") = mViewportRect.x;
        node.append_attribute("viewportY") = mViewportRect.y;
        node.append_attribute("viewportWidth") = mViewportRect.width;
        node.append_attribute("viewportHeight") = mViewportRect.height;
        node.append_attribute("active") = isActive();
    }
    
}

    

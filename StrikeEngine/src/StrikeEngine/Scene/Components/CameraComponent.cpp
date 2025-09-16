#include "CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <sstream>

namespace StrikeEngine {

    CameraComponent::CameraComponent() {
        updateProjectionMatrix();
    }

    CameraComponent::CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane)
        : mProjectionType(ProjectionType::PERSPECTIVE)
        , mFOV(fov)
        , mAspectRatio(aspectRatio)
        , mNearPlane(nearPlane)
        , mFarPlane(farPlane) {
        updateProjectionMatrix();
    }

    CameraComponent::CameraComponent(float left, float right, float bottom, float top, float nearPlane, float farPlane)
        : mProjectionType(ProjectionType::ORTHOGRAPHIC)
        , mLeft(left)
        , mRight(right)
        , mBottom(bottom)
        , mTop(top)
        , mNearPlane(nearPlane)
        , mFarPlane(farPlane) {
        updateProjectionMatrix();
    }

    void CameraComponent::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        mProjectionType = ProjectionType::PERSPECTIVE;
        mFOV = fov;
        mAspectRatio = aspectRatio;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
        mProjectionDirty = true;
    }

    void CameraComponent::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
        mProjectionType = ProjectionType::ORTHOGRAPHIC;
        mLeft = left;
        mRight = right;
        mBottom = bottom;
        mTop = top;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
        mProjectionDirty = true;
    }

    void CameraComponent::setFOV(float fov) {
        mFOV = std::clamp(fov, 1.0f, 179.0f);
        if (mProjectionType == ProjectionType::PERSPECTIVE) {
            mProjectionDirty = true;
        }
    }

    void CameraComponent::setAspectRatio(float aspectRatio) {
        mAspectRatio = std::max(aspectRatio, 0.01f);
        if (mProjectionType == ProjectionType::PERSPECTIVE) {
            mProjectionDirty = true;
        }
    }

    void CameraComponent::setNearPlane(float nearPlane) {
        mNearPlane = std::max(nearPlane, 0.001f);
        mProjectionDirty = true;
    }

    void CameraComponent::setFarPlane(float farPlane) {
        mFarPlane = std::max(farPlane, mNearPlane + 0.001f);
        mProjectionDirty = true;
    }

    void CameraComponent::setOrthographicBounds(float left, float right, float bottom, float top) {
        mLeft = left;
        mRight = right;
        mBottom = bottom;
        mTop = top;
        if (mProjectionType == ProjectionType::ORTHOGRAPHIC) {
            mProjectionDirty = true;
        }
    }

    void CameraComponent::setViewportRect(float x, float y, float width, float height) {
        mViewportRect.x = std::clamp(x, 0.0f, 1.0f);
        mViewportRect.y = std::clamp(y, 0.0f, 1.0f);
        mViewportRect.width = std::clamp(width, 0.0f, 1.0f - mViewportRect.x);
        mViewportRect.height = std::clamp(height, 0.0f, 1.0f - mViewportRect.y);
    }

    glm::mat4 CameraComponent::getProjectionMatrix() {
        if (mProjectionDirty) {
            updateProjectionMatrix();
        }
        return mProjectionMatrix;
    }

    glm::mat4 CameraComponent::getViewMatrix(const glm::vec3& position, const glm::quat& rotation) {
        glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        return glm::lookAt(position, position + forward, up);
    }

    glm::mat4 CameraComponent::getViewProjectionMatrix(const glm::vec3& position, const glm::quat& rotation)  {
        return getProjectionMatrix() * getViewMatrix(position, rotation);
    }

    CameraComponent::Frustum CameraComponent::calculateFrustum(const glm::vec3& position, const glm::quat& rotation) {
        Frustum frustum;
        glm::mat4 viewProj = getViewProjectionMatrix(position, rotation);

        // Extract frustum planes
        frustum.planes[0] = glm::vec4(
            viewProj[0][3] + viewProj[0][0],
            viewProj[1][3] + viewProj[1][0],
            viewProj[2][3] + viewProj[2][0],
            viewProj[3][3] + viewProj[3][0]
        );

        frustum.planes[1] = glm::vec4(
            viewProj[0][3] - viewProj[0][0],
            viewProj[1][3] - viewProj[1][0],
            viewProj[2][3] - viewProj[2][0],
            viewProj[3][3] - viewProj[3][0]
        );

        frustum.planes[2] = glm::vec4(
            viewProj[0][3] + viewProj[0][1],
            viewProj[1][3] + viewProj[1][1],
            viewProj[2][3] + viewProj[2][1],
            viewProj[3][3] + viewProj[3][1]
        );

        frustum.planes[3] = glm::vec4(
            viewProj[0][3] - viewProj[0][1],
            viewProj[1][3] - viewProj[1][1],
            viewProj[2][3] - viewProj[2][1],
            viewProj[3][3] - viewProj[3][1]
        );

        frustum.planes[4] = glm::vec4(
            viewProj[0][3] + viewProj[0][2],
            viewProj[1][3] + viewProj[1][2],
            viewProj[2][3] + viewProj[2][2],
            viewProj[3][3] + viewProj[3][2]
        );

        frustum.planes[5] = glm::vec4(
            viewProj[0][3] - viewProj[0][2],
            viewProj[1][3] - viewProj[1][2],
            viewProj[2][3] - viewProj[2][2],
            viewProj[3][3] - viewProj[3][2]
        );

        for (int i = 0; i < 6; i++) {
            float length = glm::length(glm::vec3(frustum.planes[i]));
            if (length > 0.0f) {
                frustum.planes[i] /= length;
            }
        }

        return frustum;
    }

    void CameraComponent::updateProjectionMatrix() {
        if (mProjectionType == ProjectionType::PERSPECTIVE) {
            mProjectionMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearPlane, mFarPlane);
        }
        else {
            mProjectionMatrix = glm::ortho(mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane);
        }
        mProjectionDirty = false;
    }

    void CameraComponent::deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
        auto projTypeIt = attributes.find("projectionType");
        if (projTypeIt != attributes.end()) {
            mProjectionType = (projTypeIt->second == "orthographic") ? ProjectionType::ORTHOGRAPHIC : ProjectionType::PERSPECTIVE;
        }

        auto nearIt = attributes.find("nearPlane");
        if (nearIt != attributes.end()) {
            mNearPlane = std::stof(nearIt->second);
        }

        auto farIt = attributes.find("farPlane");
        if (farIt != attributes.end()) {
            mFarPlane = std::stof(farIt->second);
        }

        auto fovIt = attributes.find("fov");
        if (fovIt != attributes.end()) {
            mFOV = std::stof(fovIt->second);
        }

        auto aspectIt = attributes.find("aspectRatio");
        if (aspectIt != attributes.end()) {
            mAspectRatio = std::stof(aspectIt->second);
        }

        auto leftIt = attributes.find("left");
        if (leftIt != attributes.end()) {
            mLeft = std::stof(leftIt->second);
        }

        auto rightIt = attributes.find("right");
        if (rightIt != attributes.end()) {
            mRight = std::stof(rightIt->second);
        }

        auto bottomIt = attributes.find("bottom");
        if (bottomIt != attributes.end()) {
            mBottom = std::stof(bottomIt->second);
        }

        auto topIt = attributes.find("top");
        if (topIt != attributes.end()) {
            mTop = std::stof(topIt->second);
        }

        auto orderIt = attributes.find("renderOrder");
        if (orderIt != attributes.end()) {
            mRenderOrder = std::stoi(orderIt->second);
        }

        auto viewportXIt = attributes.find("viewportX");
        if (viewportXIt != attributes.end()) {
            mViewportRect.x = std::stof(viewportXIt->second);
        }

        auto viewportYIt = attributes.find("viewportY");
        if (viewportYIt != attributes.end()) {
            mViewportRect.y = std::stof(viewportYIt->second);
        }

        auto viewportWidthIt = attributes.find("viewportWidth");
        if (viewportWidthIt != attributes.end()) {
            mViewportRect.width = std::stof(viewportWidthIt->second);
        }

        auto viewportHeightIt = attributes.find("viewportHeight");
        if (viewportHeightIt != attributes.end()) {
            mViewportRect.height = std::stof(viewportHeightIt->second);
        }

        auto renderToTextureIt = attributes.find("renderToTexture");
        if (renderToTextureIt != attributes.end() && renderToTextureIt->second == "true") {
            auto widthIt = attributes.find("textureWidth");
            auto heightIt = attributes.find("textureHeight");
            if (widthIt != attributes.end() && heightIt != attributes.end()) {
                uint32_t width = std::stoi(widthIt->second);
                uint32_t height = std::stoi(heightIt->second);
                //enableRenderToTexture(width, height);
            }
        }

        setViewportRect(mViewportRect.x, mViewportRect.y, mViewportRect.width, mViewportRect.height);
        mProjectionDirty = true;
    }

    void CameraComponent::serialize(pugi::xml_node& node) const {
        node.append_attribute("projectionType") = (mProjectionType == ProjectionType::ORTHOGRAPHIC) ? "orthographic" : "perspective";
        node.append_attribute("nearPlane") = mNearPlane;
        node.append_attribute("farPlane") = mFarPlane;

        if (mProjectionType == ProjectionType::PERSPECTIVE) {
            node.append_attribute("fov") = mFOV;
            node.append_attribute("aspectRatio") = mAspectRatio;
        }

        if (mProjectionType == ProjectionType::ORTHOGRAPHIC) {
            node.append_attribute("left") = mLeft;
            node.append_attribute("right") = mRight;
            node.append_attribute("bottom") = mBottom;
            node.append_attribute("top") = mTop;
        }

        if (mRenderOrder != 0) {
            node.append_attribute("renderOrder") = mRenderOrder;
        }

        node.append_attribute("viewportX") = mViewportRect.x;
        node.append_attribute("viewportY") = mViewportRect.y;
        node.append_attribute("viewportWidth") = mViewportRect.width;
        node.append_attribute("viewportHeight") = mViewportRect.height;

    }

}
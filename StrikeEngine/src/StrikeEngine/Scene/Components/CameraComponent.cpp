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

    void CameraComponent::deserialize(const pugi::xml_node& node) {
        // Projection type
        if (auto attr = node.attribute("projectionType")) {
            std::string value = attr.as_string();
            mProjectionType = (value == "orthographic") ? ProjectionType::ORTHOGRAPHIC : ProjectionType::PERSPECTIVE;
        }

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

        // Orthographic bounds
        if (auto attr = node.attribute("left")) {
            mLeft = attr.as_float();
        }
        if (auto attr = node.attribute("right")) {
            mRight = attr.as_float();
        }
        if (auto attr = node.attribute("bottom")) {
            mBottom = attr.as_float();
        }
        if (auto attr = node.attribute("top")) {
            mTop = attr.as_float();
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

        // Render-to-texture
        if (auto attr = node.attribute("renderToTexture")) {
            if (std::string(attr.as_string()) == "true") {
                auto widthAttr = node.attribute("textureWidth");
                auto heightAttr = node.attribute("textureHeight");
                if (widthAttr && heightAttr) {
                    uint32_t width = widthAttr.as_int();
                    uint32_t height = heightAttr.as_int();
                    // enableRenderToTexture(width, height);
                }
            }
        }

        // Final setup
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
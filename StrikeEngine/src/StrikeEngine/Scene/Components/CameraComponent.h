#pragma once
#include "Component.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"

#include <string>
#include <unordered_map>
#include <pugixml.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {

    enum class ProjectionType {
        PERSPECTIVE = 0,
        ORTHOGRAPHIC = 1
    };

    struct Rect {
        float x;
        float y;
        float width;
        float height;
    };

    class CameraComponent : public Component {
    public:
        CameraComponent();
        CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane);
        CameraComponent(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        // Static type name for registration
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "camera";
            return typeName;
        }

        // Virtual method implementation
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        // Serialization methods
        void deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Projection settings
        void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
        void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        // Getters
        ProjectionType getProjectionType() const { return mProjectionType; }
        float getFOV() { return mFOV; }
        float getAspectRatio() { return mAspectRatio; }
        float getNearPlane() { return mNearPlane; }
        float getFarPlane() { return mFarPlane; }

        // Orthographic specific getters
        float getLeft() const { return mLeft; }
        float getRight() const { return mRight; }
        float getBottom() const { return mBottom; }
        float getTop() const { return mTop; }

        // Viewport rectangle (Unity-like)
        void setViewportRect(float x, float y, float width, float height);
        const Rect& getViewportRect() const { return mViewportRect; }

        // Setters
        void setFOV(float fov);
        void setAspectRatio(float aspectRatio);
        void setNearPlane(float nearPlane);
        void setFarPlane(float farPlane);
        void setOrthographicBounds(float left, float right, float bottom, float top);

        // Matrix calculations
        glm::mat4 getProjectionMatrix();
        glm::mat4 getViewMatrix(const glm::vec3& position, const glm::quat& rotation);
        glm::mat4 getViewProjectionMatrix(const glm::vec3& position, const glm::quat& rotation);

        int getRenderOrder() const { return mRenderOrder; }
        void setRenderOrder(int order) { mRenderOrder = order; }

        // View frustum
        struct Frustum {
            glm::vec4 planes[6]; // left, right, bottom, top, near, far
        };
        Frustum calculateFrustum(const glm::vec3& position, const glm::quat& rotation);

        //void enableRenderToTexture(uint32_t width, uint32_t height);
        //void disableRenderToTexture();

    private:
        void updateProjectionMatrix();

        ProjectionType mProjectionType = ProjectionType::PERSPECTIVE;

        // Perspective parameters
        float mFOV = 45.0f;
        float mAspectRatio = 16.0f / 9.0f;

        // Orthographic parameters
        float mLeft = -10.0f;
        float mRight = 10.0f;
        float mBottom = -10.0f;
        float mTop = 10.0f;

        // Common parameters
        float mNearPlane = 0.1f;
        float mFarPlane = 1000.0f;

        int mRenderOrder = 0;

        Rect mViewportRect = { 0.0f, 0.0f, 1.0f, 1.0f };

        // Cached matrices
        mutable glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
        mutable bool mProjectionDirty = true;

    };

}
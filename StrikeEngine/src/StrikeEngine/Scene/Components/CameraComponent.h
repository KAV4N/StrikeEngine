#pragma once
#include "Component.h"

#include <string>
#include <unordered_map>
#include <pugixml.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {

    class FrameBuffer;

    class CameraComponent : public Component {
    public:
        CameraComponent();
        CameraComponent(float fov, float nearPlane, float farPlane);
        
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "camera";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        struct Frustum {
            glm::vec4 planes[6];
        };

        struct Rect {
            float x;
            float y;
            float width;
            float height;
        };

        // Serialization methods
        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Projection settings
        void setPerspective(float fov, float nearPlane, float farPlane);

        // Getters
        float getFOV() { return mFOV; }
        float getNearPlane() { return mNearPlane; }
        float getFarPlane() { return mFarPlane; }

        float getNearPlane() const { return mNearPlane; }
        float getFarPlane() const { return mFarPlane; }

        const Frustum& getFrustum() const { return mFrustum; }

        const glm::vec3& getRight() const { return mRight; }
        const glm::vec3& getUp() const { return mUp; }
        const glm::vec3& getForward() const { return mForward; }

        // Viewport rectangle (Unity-like)
        void setViewportRect(float x, float y, float width, float height);
        const Rect& getViewportRect() const { return mViewportRect; }

        // Setters
        void setFOV(float fov);
        void setNearPlane(float nearPlane);
        void setFarPlane(float farPlane);

        // Matrix calculations
        glm::mat4 getProjectionMatrix();
        glm::mat4 getViewMatrix();
        glm::mat4 getViewProjectionMatrix();

        glm::mat4 getProjectionMatrix() const;
        glm::mat4 getViewMatrix() const;
        glm::mat4 getViewProjectionMatrix() const;

        int getRenderOrder() const { return mRenderOrder; }
        void setRenderOrder(int order) { mRenderOrder = order; }

        void update(const glm::mat4& worldMatrix, uint32_t width, uint32_t height);

    private:
        void updateProjectionMatrix(uint32_t width, uint32_t height);
        void updateViewMatrix(const glm::mat4& worldMatrix);
        void updateViewProjectionMatrix();
        void calculateFrustum();

    private:
        // Perspective parameters
        float mFOV = 45.0f;
        float mNearPlane = 0.1f;
        float mFarPlane = 1000.0f;

        int mRenderOrder = 0;

        Rect mViewportRect = { 0.0f, 0.0f, 1.0f, 1.0f };

        Frustum mFrustum;

        glm::vec3 mRight;
        glm::vec3 mUp;
        glm::vec3 mForward;

        glm::mat4 mProjectionMatrix;
        glm::mat4 mViewMatrix;
        glm::mat4 mViewProjectionMatrix;
    };

}
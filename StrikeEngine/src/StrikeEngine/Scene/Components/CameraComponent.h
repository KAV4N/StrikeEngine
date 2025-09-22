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



    class CameraComponent : public Component {
    public:
        CameraComponent();
        CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane);

        // Static type name for registration
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "camera";
            return typeName;
        }

        // Virtual method implementation
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
        void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

        // Getters
        float getFOV() { return mFOV; }
        float getAspectRatio() { return mAspectRatio; }
        float getNearPlane() { return mNearPlane; }
        float getFarPlane() { return mFarPlane; }
        const Frustum& getFrustum() const { return mFrustum; }

        // Viewport rectangle (Unity-like)
        void setViewportRect(float x, float y, float width, float height);
        const Rect& getViewportRect() const { return mViewportRect; }

        // Setters
        void setFOV(float fov);
        void setAspectRatio(float aspectRatio);
        void setNearPlane(float nearPlane);
        void setFarPlane(float farPlane);

        // Matrix calculations
        glm::mat4 getProjectionMatrix();
        glm::mat4 getViewMatrix();
        glm::mat4 getViewProjectionMatrix();

        int getRenderOrder() const { return mRenderOrder; }
        void setRenderOrder(int order) { mRenderOrder = order; }



    private:
        friend class SceneGraph;

        void update(const glm::vec3& position, const glm::quat& rotation);
        void updateProjectionMatrix(const glm::vec3& position, const glm::quat& rotation);
        void updateViewMatrix(const glm::vec3& position, const glm::quat& rotation);
        void updateViewProjectionMatrix(const glm::vec3& position, const glm::quat& rotation);
        void calculateFrustum(const glm::vec3& position, const glm::quat& rotation);

    private:
        // Perspective parameters
        float mAspectRatio = 16.0f / 9.0f;

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
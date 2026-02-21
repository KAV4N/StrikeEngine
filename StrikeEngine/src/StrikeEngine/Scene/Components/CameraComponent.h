#pragma once
#include "Component.h"

#include <string>
#include <unordered_map>
#include <pugixml.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Strike {

    class FrameBuffer;

    /**
     * @brief Component for rendering the scene from a specific viewpoint.
     * 
     * The CameraComponent defines the perspective projection, view frustum, and viewport
     * for rendering. It maintains view and projection matrices and supports frustum culling.
     * Multiple cameras can exist with different render orders.
     */
    class CameraComponent : public Component {
    public:
        /**
         * @brief Default constructor
         * 
         * Initializes camera with default FOV (45°), near plane (0.1), and far plane (1000)
         */
        CameraComponent();
        
        /**
         * @brief Construct camera with custom projection parameters
         * 
         * @param fov Field of view in degrees
         * @param nearPlane Near clipping plane distance
         * @param farPlane Far clipping plane distance
         */
        CameraComponent(float fov, float nearPlane, float farPlane);
        
        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "camera"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "camera";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "camera"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Frustum structure for culling tests.
         * 
         * Contains 6 planes representing the camera's view frustum.
         */
        struct Frustum {
            glm::vec4 planes[6]; ///< Frustum planes (left, right, bottom, top, near, far)
        };

        /**
         * @brief Rectangle structure for viewport definition.
         * 
         * Defines a normalized viewport rectangle (0-1 range).
         */
        struct Rect {
            float x;      ///< Normalized X position (0-1)
            float y;      ///< Normalized Y position (0-1)
            float width;  ///< Normalized width (0-1)
            float height; ///< Normalized height (0-1)
        };

        /**
         * @brief Deserialize camera data from XML node
         * 
         * @param node The XML node containing camera configuration
         */
        void deserialize(const pugi::xml_node& node) override;

        /**
         * @brief Set perspective projection parameters
         * 
         * @param fov Field of view in degrees
         * @param nearPlane Near clipping plane distance
         * @param farPlane Far clipping plane distance
         */
        void setPerspective(float fov, float nearPlane, float farPlane);

        /**
         * @brief Get the field of view
         * 
         * @return Field of view in degrees
         */
        float getFOV() const { return mFOV; }
        
        /**
         * @brief Get the field of view (non-const)
         * 
         * @return Field of view in degrees
         */
        float getFOV() { return mFOV; }
        
        /**
         * @brief Get the near clipping plane distance
         * 
         * @return Near plane distance
         */
        float getNearPlane() { return mNearPlane; }
        
        /**
         * @brief Get the far clipping plane distance
         * 
         * @return Far plane distance
         */
        float getFarPlane() { return mFarPlane; }

        /**
         * @brief Get the near clipping plane distance (const)
         * 
         * @return Near plane distance
         */
        float getNearPlane() const { return mNearPlane; }
        
        /**
         * @brief Get the far clipping plane distance (const)
         * 
         * @return Far plane distance
         */
        float getFarPlane() const { return mFarPlane; }

        /**
         * @brief Get the view frustum
         * 
         * @return Constant reference to the frustum structure
         * @note Frustum is updated when update() is called
         */
        const Frustum& getFrustum() const { return mFrustum; }

        /**
         * @brief Set the viewport rectangle
         * 
         * @param x Normalized X position (0-1)
         * @param y Normalized Y position (0-1)
         * @param width Normalized width (0-1)
         * @param height Normalized height (0-1)
         * @note Values are normalized to the window size
         */
        void setViewportRect(float x, float y, float width, float height);
        
        /**
         * @brief Get the viewport rectangle
         * 
         * @return Constant reference to the viewport rectangle
         */
        const Rect& getViewportRect() const { return mViewportRect; }

        /**
         * @brief Set the field of view
         * 
         * @param fov Field of view in degrees
         */
        void setFOV(float fov);
        
        /**
         * @brief Set the near clipping plane distance
         * 
         * @param nearPlane Near plane distance
         */
        void setNearPlane(float nearPlane);
        
        /**
         * @brief Set the far clipping plane distance
         * 
         * @param farPlane Far plane distance
         */
        void setFarPlane(float farPlane);
        
        /**
         * @brief Get the current aspect ratio
         * 
         * @return Current aspect ratio (width/height)
         */
        float getAspectRatio() const;

        /**
         * @brief Get the projection matrix (non-const)
         * 
         * @return The 4x4 projection matrix
         */
        glm::mat4 getProjectionMatrix();
        
        /**
         * @brief Get the view matrix (non-const)
         * 
         * @return The 4x4 view matrix
         */
        glm::mat4 getViewMatrix();
        
        /**
         * @brief Get the combined view-projection matrix (non-const)
         * 
         * @return The 4x4 view-projection matrix
         */
        glm::mat4 getViewProjectionMatrix();

        /**
         * @brief Get the projection matrix (const)
         * 
         * @return The 4x4 projection matrix
         */
        glm::mat4 getProjectionMatrix() const;
        
        /**
         * @brief Get the view matrix (const)
         * 
         * @return The 4x4 view matrix
         */
        glm::mat4 getViewMatrix() const;
        
        /**
         * @brief Get the combined view-projection matrix (const)
         * 
         * @return The 4x4 view-projection matrix
         */
        glm::mat4 getViewProjectionMatrix() const;

        /**
         * @brief Get the camera's render order
         * 
         * @return Render order value (lower values render first)
         */
        int getRenderOrder() const { return mRenderOrder; }
        
        /**
         * @brief Set the camera's render order
         * 
         * @param order Render order value (lower values render first)
         * @note Used for multi-camera setups
         */
        void setRenderOrder(int order) { mRenderOrder = order; }

        /**
         * @brief Orient the camera to look at a target position
         * 
         * @param eye Camera position
         * @param center Target position to look at
         * @param up Up direction vector (default: world up)
         */
        void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

    private:
        /**
         * @brief Update camera matrices and frustum
         * 
         * @param worldMatrix Entity's world transformation matrix
         * @param width Viewport/window width in pixels
         * @param height Viewport/window height in pixels
         * @note Should be called each frame to update camera state
         */
        void update(const glm::mat4& worldMatrix, uint32_t width, uint32_t height);

        /**
         * @brief Update the projection matrix based on viewport size
         * 
         * @param width Viewport width in pixels
         * @param height Viewport height in pixels
         */
        void updateProjectionMatrix(uint32_t width, uint32_t height);
        
        /**
         * @brief Update the view matrix from world transformation
         * 
         * @param worldMatrix Entity's world transformation matrix
         */
        void updateViewMatrix(const glm::mat4& worldMatrix);
        
        /**
         * @brief Update the combined view-projection matrix
         */
        void updateViewProjectionMatrix();
        
        /**
         * @brief Calculate frustum planes from view-projection matrix
         */
        void calculateFrustum();

    private:
        friend class Scene;
        friend class RenderSystem;

        float mFOV = 45.0f;                ///< Field of view in degrees
        float mNearPlane = 0.1f;           ///< Near clipping plane distance
        float mFarPlane = 1000.0f;         ///< Far clipping plane distance

        int mRenderOrder = 0;              ///< Camera render order

        Rect mViewportRect = { 0.0f, 0.0f, 1.0f, 1.0f }; ///< Normalized viewport rectangle

        Frustum mFrustum;                  ///< View frustum for culling

        glm::mat4 mProjectionMatrix;       ///< Projection matrix
        glm::mat4 mViewMatrix;             ///< View matrix
        glm::mat4 mViewProjectionMatrix;   ///< Combined view-projection matrix

        float mAspectRatio = 1.0f;         ///< Current aspect ratio
    };

}
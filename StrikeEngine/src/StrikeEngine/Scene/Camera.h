#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    class Camera {
    public:
        Camera(float aspectRatio, float fov, float nearPlane, float farPlane);

        // Existing setters
        void SetAspectRatio(float aspectRatio);
        void SetFOV(float fov);
        void SetClipPlanes(float nearPlane, float farPlane);
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);

        // New setters
        void SetNearPlane(float nearPlane);
        void SetFarPlane(float farPlane);

        // Existing getters
        const glm::mat4& GetProjectionMatrix() const;
        const glm::mat4& GetViewMatrix() const;
        glm::vec3 GetForwardDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetUpDirection() const;
        float GetAspectRatio() const;

        // New getters
        float GetFOV() const;
        float GetNearPlane() const;
        float GetFarPlane() const;
        const glm::vec3& GetPosition() const;
        const glm::vec3& GetRotation() const;
        const glm::vec3& GetWorldUp() const;

    private:
        void UpdateProjectionMatrix();
        void UpdateViewMatrix();

        float m_AspectRatio;
        float m_Fov, m_NearPlane, m_FarPlane;
        glm::vec3 m_Position, m_Rotation;
        glm::vec3 m_Forward, m_Right, m_Up;
        const glm::vec3 m_WorldUp;
        glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
    };
}
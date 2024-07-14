#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine
{
    class Camera
    {
    public:
        Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

        void SetPosition(const glm::vec3& position);
        void SetRotation(float pitch, float yaw);

    private:
        void UpdateViewMatrix();

    private:
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;

        glm::vec3 m_Position;
        float m_Pitch;
        float m_Yaw;
    };
}

#include "strikepch.h"
#include "Camera.h"

namespace StrikeEngine
{
    Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_Position(0.0f, 0.0f, 0.0f), m_Pitch(0.0f), m_Yaw(0.0f)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        UpdateViewMatrix();
    }

    void Camera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

    void Camera::SetRotation(float pitch, float yaw)
    {
        m_Pitch = pitch;
        m_Yaw = yaw;
        UpdateViewMatrix();
    }

    void Camera::UpdateViewMatrix()
    {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        rotation = glm::rotate(rotation, glm::radians(m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Position);

        m_ViewMatrix = rotation * translation;
    }
}

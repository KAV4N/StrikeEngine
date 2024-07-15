#include "strikepch.h"
#include "Camera.h"

namespace StrikeEngine {
    Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f, 0.0f, 0.0f) {
        m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        UpdateViewMatrix();
    }

    void Camera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        UpdateViewMatrix();
    }

    void Camera::SetRotation(const glm::vec3& rotation) {
        m_Rotation = rotation;
        UpdateViewMatrix();
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return m_ViewMatrix;
    }

    glm::mat4 Camera::GetProjectionMatrix() const {
        return m_ProjectionMatrix;
    }

    void Camera::UpdateViewMatrix() {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
        transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
        m_ViewMatrix = glm::inverse(transform);
    }
}

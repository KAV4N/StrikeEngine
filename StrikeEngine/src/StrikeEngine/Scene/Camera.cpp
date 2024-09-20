#include "strikepch.h"
#include "Camera.h"

namespace StrikeEngine {
    Camera::Camera(float aspectRatio, float fov, float nearPlane, float farPlane)
        : m_AspectRatio(aspectRatio), m_Fov(fov), m_NearPlane(nearPlane), m_FarPlane(farPlane),
        m_Position(0.0f), m_Rotation(0.0f), m_WorldUp(0.0f, 1.0f, 0.0f)
    {
        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }

    // Existing setters
    void Camera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        UpdateProjectionMatrix();
    }

    void Camera::SetFOV(float fov) {
        m_Fov = fov;
        UpdateProjectionMatrix();
    }

    void Camera::SetClipPlanes(float nearPlane, float farPlane) {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    void Camera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        UpdateViewMatrix();
    }

    void Camera::SetRotation(const glm::vec3& rotation) {
        m_Rotation = rotation;
        UpdateViewMatrix();
    }

    // New setters
    void Camera::SetNearPlane(float nearPlane) {
        m_NearPlane = nearPlane;
        UpdateProjectionMatrix();
    }

    void Camera::SetFarPlane(float farPlane) {
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }

    // Existing getters
    const glm::mat4& Camera::GetProjectionMatrix() const {
        return m_ProjectionMatrix;
    }

    const glm::mat4& Camera::GetViewMatrix() const {
        return m_ViewMatrix;
    }

    glm::vec3 Camera::GetForwardDirection() const {
        return m_Forward;
    }

    glm::vec3 Camera::GetRightDirection() const {
        return m_Right;
    }

    glm::vec3 Camera::GetUpDirection() const {
        return m_Up;
    }

    float Camera::GetAspectRatio() const {
        return m_AspectRatio;
    }

    // New getters
    float Camera::GetFOV() const {
        return m_Fov;
    }

    float Camera::GetNearPlane() const {
        return m_NearPlane;
    }

    float Camera::GetFarPlane() const {
        return m_FarPlane;
    }

    const glm::vec3& Camera::GetPosition() const {
        return m_Position;
    }

    const glm::vec3& Camera::GetRotation() const {
        return m_Rotation;
    }

    const glm::vec3& Camera::GetWorldUp() const {
        return m_WorldUp;
    }

    // Existing private methods
    void Camera::UpdateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), GetAspectRatio(), m_NearPlane, m_FarPlane);
    }

    void Camera::UpdateViewMatrix() {
        m_Forward = glm::normalize(glm::vec3(
            cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)),
            sin(glm::radians(m_Rotation.x)),
            sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x))
        ));
        m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
    }
}
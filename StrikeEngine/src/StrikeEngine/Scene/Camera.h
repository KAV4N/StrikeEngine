/*
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {
    class Camera {
    public:
        Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
            : m_Position(0.0f, 0.0f, 0.0f),
            m_Rotation(0.0f, 0.0f, 0.0f),
            m_Up(0.0f, 1.0f, 0.0f),
            m_Fov(fov),
            m_AspectRatio(aspectRatio),
            m_NearPlane(nearPlane),
            m_FarPlane(farPlane)
        {
            UpdateViewMatrix();
        }

        void SetPosition(const glm::vec3& position) {
            m_Position = position;
            UpdateViewMatrix();
        }

        void SetRotation(const glm::vec3& rotation) {
            m_Rotation = rotation;
            UpdateViewMatrix();
        }

        void SetUp(const glm::vec3& up) {
            m_Up = up;
            UpdateViewMatrix();
        }

        inline glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
        inline glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }

        void MoveForward(float distance) {
            m_Position += GetForwardDirection() * distance;
            UpdateViewMatrix();
        }

        void MoveBackward(float distance) {
            m_Position -= GetForwardDirection() * distance;
            UpdateViewMatrix();
        }

        void MoveLeft(float distance) {
            m_Position -= GetRightDirection() * distance;
            UpdateViewMatrix();
        }

        void MoveRight(float distance) {
            m_Position += GetRightDirection() * distance;
            UpdateViewMatrix();
        }

        void RotateYaw(float angle) {
            m_Rotation.y += angle;
            UpdateViewMatrix();
        }

        void RotatePitch(float angle) {
            m_Rotation.x += angle;
            UpdateViewMatrix();
        }

        inline glm::vec3 GetPosition() const { return m_Position; }
        inline glm::vec3 GetOrientation() const { return m_Rotation; }
        inline glm::vec3 GetUp() const { return m_Up; }

    private:
        float m_Fov;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;

        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Up;

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_CameraMatrix;

        void UpdateViewMatrix() {
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Rotation, m_Up);
            m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane);
            m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        glm::vec3 GetForwardDirection() const {
            return glm::normalize(glm::vec3(
                cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)),
                sin(glm::radians(m_Rotation.x)),
                sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x))
            ));
        }

        inline glm::vec3 GetRightDirection() const {
            return glm::normalize(glm::cross(GetForwardDirection(), m_Up));
        }
    };
}

*/

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    class Camera {
    public:
        Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
            : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f, 0.0f, 0.0f)
        {
            m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
            UpdateViewMatrix();
        }

        void SetPosition(const glm::vec3& position)
        {
            m_Position = position;
            UpdateViewMatrix();
        }

        void SetRotation(const glm::vec3& rotation)
        {
            m_Rotation = rotation;
            UpdateViewMatrix();
        }

        inline glm::mat4 GetViewMatrix() { return m_ViewMatrix; }

        inline glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }


        void MoveForward(float distance)
        {
            m_Position += GetForwardDirection() * distance;
            UpdateViewMatrix();
        }

        void MoveBackward(float distance)
        {
            m_Position -= GetForwardDirection() * distance;
            UpdateViewMatrix();
        }

        void MoveLeft(float distance)
        {
            m_Position -= GetRightDirection() * distance;
            UpdateViewMatrix();
        }

        void MoveRight(float distance)
        {
            m_Position += GetRightDirection() * distance;
            UpdateViewMatrix();
        }

        void RotateYaw(float angle)
        {
            m_Rotation.y += angle;
            UpdateViewMatrix();
        }

        void RotatePitch(float angle)
        {
            m_Rotation.x += angle;
            UpdateViewMatrix();
        }

        inline glm::vec3 GetPosition() { return m_Position; }

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        void UpdateViewMatrix() {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
            transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
            transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
            transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
            m_ViewMatrix = glm::inverse(transform);
        }

        glm::vec3 GetForwardDirection() const
        {
            return glm::normalize(glm::vec3(
                cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)),
                sin(glm::radians(m_Rotation.x)),
                sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x))
            ));
        }

        inline glm::vec3 GetRightDirection() const { return glm::normalize(glm::cross(GetForwardDirection(), glm::vec3(0.0f, 1.0f, 0.0f))); }
    };
}

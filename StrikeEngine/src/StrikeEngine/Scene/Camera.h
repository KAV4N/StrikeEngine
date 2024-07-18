/*
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    class Camera {
    public:
        Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix();

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;

        glm::mat4 m_ProjectionMatrix;

        void UpdateViewMatrix();
        glm::mat4 m_ViewMatrix;
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

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        void UpdateViewMatrix() {
            /*
            glm::vec3 front;
            front.x = cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
            front.y = sin(glm::radians(m_Rotation.x));
            front.z = sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
            front = glm::normalize(front);

            glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 up = glm::normalize(glm::cross(right, front));

            m_ViewMatrix = glm::lookAt(m_Position, m_Position + front, up);
            */
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

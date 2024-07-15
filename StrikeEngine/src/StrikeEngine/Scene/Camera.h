#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    class Camera {
    public:
        Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;

        glm::mat4 m_ProjectionMatrix;

        void UpdateViewMatrix();
        glm::mat4 m_ViewMatrix;
    };
}

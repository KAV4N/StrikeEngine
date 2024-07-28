#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    struct CameraComponent {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;

        CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane)
        {
            ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
            UpdateViewMatrix();
        }

        void UpdateViewMatrix() {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position);
            transform = glm::rotate(transform, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
            transform = glm::rotate(transform, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
            transform = glm::rotate(transform, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
            ViewMatrix = glm::inverse(transform);
        }

        glm::vec3 GetForwardDirection() const {
            return glm::normalize(glm::vec3(
                cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x)),
                sin(glm::radians(Rotation.x)),
                sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x))
            ));
        }

        glm::vec3 GetRightDirection() const {
            return glm::normalize(glm::cross(GetForwardDirection(), glm::vec3(0.0f, 1.0f, 0.0f)));
        }
    };
}

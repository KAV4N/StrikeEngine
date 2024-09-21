#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "StrikeEngine/Scene/Components/TransformComponent.h"
#include "StrikeEngine/Graphics/Core/FrameBuffer.h"

namespace StrikeEngine {
  
    struct CameraComponent {
    public:
        GLuint resX, resY;

        glm::vec3 Right;
        glm::vec3 Up;
        glm::vec3 Forward;

        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;

        const glm::vec3 WorldUp = glm::vec3(0, 1, 0);

        float fov;
        float nearPlane;
        float farPlane;

        CameraComponent(GLuint resX, GLuint resY, float fov, float nearPlane, float farPlane, int numClusters = 4)
            : resX(resX), resY(resY), fov(fov), nearPlane(nearPlane), farPlane(farPlane)
        {

            ProjectionMatrix = glm::perspective(glm::radians(fov), (static_cast<float>(resX) / resY) , nearPlane, farPlane);
            UpdateViewMatrix(glm::vec3(0.0f), glm::vec3(0.0f));
        }

        ~CameraComponent() {

        }

        void UpdateViewMatrix(const glm::vec3& position, const glm::vec3& rotation) {
            Forward = glm::normalize(glm::vec3(
                cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x)),
                sin(glm::radians(rotation.x)),
                sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x))
            ));
            Right = glm::normalize(glm::cross(Forward, WorldUp));
            Up = glm::normalize(glm::cross(Right, Forward));

            ViewMatrix = glm::lookAt(position, position + Forward, Up);
        }

        glm::vec3 GetForwardDirection() const {
            return Forward;
        }

        glm::vec3 GetRightDirection() const {
            return Right;
        }

        glm::vec3 GetUpDirection() const {
            return Up;
        }

        float GetAspectRatio() const {
            return (static_cast<float>(resX) / resY);
        }

    };
}
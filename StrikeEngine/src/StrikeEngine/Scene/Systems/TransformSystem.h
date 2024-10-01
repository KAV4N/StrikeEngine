#pragma once

#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Components/TransformComponent.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Scene/Entity.h"

namespace StrikeEngine {
    class TransformSystem {
    public:
        static glm::mat4 CalculateTransformMatrix(const TransformComponent& transformComponent) {

            const auto& position = transformComponent.Position;
            const auto& rotation = transformComponent.Rotation;
            const auto& scale = transformComponent.Scale;

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0f), scale);
            return transform;
        }

        static glm::mat4 CalculateRotationMatrix(const TransformComponent& transformComponent) {
            const auto& rotation = transformComponent.Rotation;
            glm::mat4 rotationMatrix = glm::mat4(1.0f);
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0))
                            * glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0))
                            * glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

            return rotationMatrix;
        }

    };
}
#pragma once

#include "StrikeEngine/Graphics/Core/Model.h"
#include <glm/glm.hpp>

namespace StrikeEngine {

    struct TransformComponent {

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        TransformComponent(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 sca = glm::vec3(1.0f))
            : position(pos),
            rotation(rot),
            scale(sca)
        {}

        glm::mat4 CalculateTransformMatrix() {

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0f), scale);
            return transform;
        }

    };
}

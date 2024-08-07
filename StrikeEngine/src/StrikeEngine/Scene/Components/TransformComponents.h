#pragma once

#include "StrikeEngine/Renderer/Core/Model.h"
#include <glm/glm.hpp>

namespace StrikeEngine {


    struct PositionComponent {
        glm::vec3 position;
        PositionComponent(glm::vec3 position = glm::vec3(0.0f)) : position(position) {}
    };

    struct RotationComponent {
        glm::vec3 rotation;
        RotationComponent(glm::vec3 rotation = glm::vec3(0.0f)) : rotation(rotation) {}
    };

    struct ScaleComponent {
        glm::vec3 scale;
        ScaleComponent(glm::vec3 scale = glm::vec3(1.0f)) : scale(scale) {}
    };

    struct TransformComponent {
        glm::mat4 transformationMatrix;
    };
}

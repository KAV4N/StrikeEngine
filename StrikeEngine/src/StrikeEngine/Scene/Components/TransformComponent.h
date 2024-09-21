#pragma once

#include "StrikeEngine/Graphics/Core/Model.h"
#include <glm/glm.hpp>

namespace StrikeEngine {

    struct TransformComponent {

        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;

        TransformComponent(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 sca = glm::vec3(1.0f))
            : Position(pos),
            Rotation(rot),
            Scale(sca)
        {}

    };
}

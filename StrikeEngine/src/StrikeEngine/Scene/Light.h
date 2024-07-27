#pragma once

#include <glm/glm.hpp>

namespace StrikeEngine {

    struct Light {
        glm::vec3 position = glm::vec3(0.0f);
        float radius = 0.f;
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
    };


}

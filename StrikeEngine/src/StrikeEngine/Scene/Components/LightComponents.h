#pragma once

#include <glm/glm.hpp>

namespace StrikeEngine {

    struct DirectionalLightComponent  {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
    };

    struct PointLightComponent {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
        float radius = 1.0f;
    };

    struct SpotLightComponent {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
        float cutoff = glm::cos(glm::radians(30.0f));
    };
    




}
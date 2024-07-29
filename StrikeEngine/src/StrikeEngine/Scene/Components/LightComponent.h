#pragma once
#include <glm/glm.hpp>

namespace StrikeEngine {

    enum class LightType {
        Directional,
        Point,
        Spot
    };

    struct LightComponent {
        LightType type;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;

        // Attenuation factors
        float constant;
        float linear;
        float quadratic;

        // Spotlight properties
        float cutOff;
        float outerCutOff;

        LightComponent()
            : type(LightType::Point),
            position(0.0f),
            direction(0.0f, -1.0f, 0.0f),
            color(1.0f),
            intensity(1.0f),
            constant(1.0f),
            linear(0.09f),
            quadratic(0.032f),
            cutOff(glm::cos(glm::radians(12.5f))),
            outerCutOff(glm::cos(glm::radians(17.5f))) {}
    };

} 
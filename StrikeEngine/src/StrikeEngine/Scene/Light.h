#pragma once

#include <glm/glm.hpp>

namespace StrikeEngine {

    struct alignas(16) DirectionalLight {
        glm::vec3 direction; // 12 bytes
        float padding1;      // 4 bytes padding
        glm::vec3 color;     // 12 bytes
        float intensity;     // 4 bytes


        DirectionalLight(glm::vec3 dir = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 col = glm::vec3(1.0f, 1.0f, 1.0f),
            float inten = 1.0f)
            : direction(dir), padding1(0.0f), color(col), intensity(inten) {}
    };

    struct alignas(16) PointLight {
        glm::vec3 position; // 12 bytes
        float padding1;     // 4 bytes padding
        glm::vec3 color;    // 12 bytes
        float intensity;    // 4 bytes
        float radius;       // 4 bytes
        float padding2;     // 4 bytes padding to maintain 16-byte alignment


        PointLight(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 col = glm::vec3(1.0f, 1.0f, 1.0f),
            float inten = 1.0f,
            float rad = 1.0f)
            : position(pos), padding1(0.0f), color(col), intensity(inten), radius(rad), padding2(0.0f) {}
    };

    struct alignas(16) SpotLight {
        glm::vec3 position; // 12 bytes
        float padding1;     // 4 bytes padding
        glm::vec3 direction; // 12 bytes
        float cutoff;        // 4 bytes
        glm::vec3 color;     // 12 bytes
        float intensity;     // 4 bytes


        SpotLight(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 dir = glm::vec3(0.0f, -1.0f, 0.0f),
            float cut = cos(glm::radians(30.0f)),
            glm::vec3 col = glm::vec3(1.0f, 1.0f, 1.0f),
            float inten = 1.0f)
            : position(pos), padding1(0.0f), direction(dir), cutoff(cut), color(col), intensity(inten) {}
    };


    
}

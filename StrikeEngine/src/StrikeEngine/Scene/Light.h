#pragma once

#include <glm/glm.hpp>

namespace StrikeEngine {
    /*
    class Light {
    public:
        Light(glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), float intensity = 1.0f)
            : m_Position(position), m_Color(color), m_Intensity(intensity) {}

        inline void SetPosition(const glm::vec3& position) { m_Position = position; }
        inline void SetColor(const glm::vec3& color) { m_Color = color; }
        inline void SetIntensity(float intensity) { m_Intensity = intensity; }

        inline glm::vec3 GetPosition() const { return m_Position; }
        inline glm::vec3 GetColor() const { return m_Color; }
        inline float GetIntensity() const { return m_Intensity; }


    private:
        glm::vec3 m_Position;
        glm::vec3 m_Color;
        float m_Intensity;
    };
    */

    
    struct Light {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
    };


}

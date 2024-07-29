#pragma once
#include <vector>
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Components/LightComponents.h"

namespace StrikeEngine {
    class Shader;

    class LightManager {
    public:
        static void Create();
        static LightManager* Get();
        static void Destroy();

        void CreateDirectionalLight(const glm::vec3& direction = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);

        void CreatePointLight(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f, float radius = 5.0f);

        void CreateSpotLight(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& direction = glm::vec3(0.0f), float cutoff = cos(glm::radians(30.f)), const glm::vec3& color = glm::vec3(1.0f), float intensity=1.0f);

        void BindLightsToShader();
        void SetActiveScene(Scene* activeScene);

    private:
        LightManager();
        ~LightManager();
        void CreateSSBOs();
        void UpdateSSBOs();

        static LightManager* s_Instance;
        Scene* m_ActiveScene;

        unsigned int m_DirectionalSSBO;
        unsigned int m_PointSSBO;
        unsigned int m_SpotSSBO;
        bool m_IsDirty;

        const unsigned int DIRECTIONAL_LIGHT_BUFFER_BINDING = 0;
        const unsigned int POINT_LIGHT_BUFFER_BINDING = 1;
        const unsigned int SPOT_LIGHT_BUFFER_BINDING = 2;
    };
}
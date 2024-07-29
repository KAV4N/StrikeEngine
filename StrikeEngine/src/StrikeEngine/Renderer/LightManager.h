#pragma once
#include <vector>
#include "StrikeEngine/Scene/Light.h"
#include <StrikeEngine/Scene/Scene.h>

namespace StrikeEngine {
    class Shader;

    class LightManager {
    public:
        static void Create();
        static LightManager* Get();
        static void Destroy();

        void AddDirectionalLight(const DirectionalLight& light);
        void RemoveDirectionalLight(size_t index);
        void UpdateDirectionalLight(size_t index, const DirectionalLight& light);

        void AddPointLight(const PointLight& light);
        void RemovePointLight(size_t index);
        void UpdatePointLight(size_t index, const PointLight& light);

        void AddSpotLight(const SpotLight& light);
        void RemoveSpotLight(size_t index);
        void UpdateSpotLight(size_t index, const SpotLight& light);

        void ClearDirectionalLights();
        void ClearPointLights();
        void ClearSpotLights();

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
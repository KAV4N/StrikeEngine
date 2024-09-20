#pragma once
#include <vector>
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Components/LightComponents.h"
#include "StrikeEngine/Scene/Components/ShadowCasterComponent.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include <StrikeEngine/Graphics/Core/ShadowAtlas.h>


namespace StrikeEngine {
    class Shader;

    class LightManager {
    public:
        static void Create();
        static LightManager* Get();
        static void Destroy();

        Entity CreateDirectionalLight(const glm::vec3& direction = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);
        Entity CreatePointLight(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f, float radius = 5.0f);
        Entity CreateSpotLight(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& direction = glm::vec3(0.0f), float cutoff = cos(glm::radians(30.f)), const glm::vec3& color = glm::vec3(1.0f), float intensity=1.0f);

        Entity CreateShadowCaster(Entity light);


        void BindLights();

        inline Scene* GetActiveScene() { return m_ActiveScene; }

        inline int GetPointCount() { return m_PointCount; }
        inline int GetDirectionalCount() { return m_DirectionalCount; }
        inline int GetSpotCount() { return m_SpotCount; }

        inline std::unique_ptr<ShadowAtlas>& LightManager::GetShadowAtlas() { return m_ShadowAtlas; }
        void UpdateSSBOs();
    private:
        LightManager();
        ~LightManager();
        void CreateSSBOs();

        

        void UpdateSSBOPoint(const entt::registry& registry);
        void UpdateSSBODirect(const entt::registry& registry);
        void UpdateSSBOSpot(const entt::registry& registry);


    private:
        Shader* m_ShadowMapShader;

        static LightManager* s_Instance;
        Scene* m_ActiveScene;

        std::unique_ptr<ShadowAtlas> m_ShadowAtlas;

        unsigned int m_DirectionalSSBO;
        unsigned int m_PointSSBO;
        unsigned int m_SpotSSBO;
        bool m_PointDirty, m_DirectionalDirty, m_SpotDirty;

        int m_SpotCount, m_DirectionalCount, m_PointCount;

        const unsigned int DIRECTIONAL_LIGHT_BUFFER_BINDING = 0;
        const unsigned int POINT_LIGHT_BUFFER_BINDING = 1;
        const unsigned int SPOT_LIGHT_BUFFER_BINDING = 2;
     
    };
}
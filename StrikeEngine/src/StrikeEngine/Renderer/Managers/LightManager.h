#pragma once
#include <vector>
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Components/LightComponents.h"
#include "StrikeEngine/Scene/Components/ShadowCasterComponent.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Renderer/Renderer3D/Renderer.h" 
#include <StrikeEngine/Renderer/Core/ShadowAtlas.h>


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


        void BindLightsToShader();
        void SetActiveScene(Scene* activeScene);

        void UpdateShadowMaps(std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);
        void Render(std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue, const glm::mat4& viewProjectionMatrix);
        bool IsVisible(const ModelPart::AABB& aabb, const glm::mat4& modelMatrix, const glm::mat4& lightSpaceMatrix);
        void UpdateDirectionalLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);
        void UpdatePointLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);
        void UpdateSpotLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);


        inline Scene* GetActiveScene() { return m_ActiveScene; }
        inline std::unique_ptr<ShadowAtlas>& LightManager::GetShadowAtlas() { return m_ShadowAtlas; }

    private:
        LightManager();
        ~LightManager();
        void CreateSSBOs();

        void UpdateSSBOs();

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

        const unsigned int DIRECTIONAL_LIGHT_BUFFER_BINDING = 0;
        const unsigned int POINT_LIGHT_BUFFER_BINDING = 1;
        const unsigned int SPOT_LIGHT_BUFFER_BINDING = 2;
       

    };
}
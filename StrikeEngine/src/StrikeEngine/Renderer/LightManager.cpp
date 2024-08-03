#include "strikepch.h"
#include "LightManager.h"
#include "Shader.h"
#include <glad/glad.h>
#include <StrikeEngine/Scene/Components/ShadowCasterComponent.h>

namespace StrikeEngine {

    LightManager* LightManager::s_Instance = nullptr;

    LightManager::LightManager()
        : m_DirectionalSSBO(0), m_PointSSBO(0), m_SpotSSBO(0),
        m_DirectionalDirty(false), m_PointDirty(false), m_SpotDirty(false) {
        m_ShadowMapShader = ShaderManager::Get()->GetShader("ShadowMapShader");
        CreateSSBOs();
    }

    void LightManager::Create() {
        if (!s_Instance) {
            s_Instance = new LightManager();
        }
    }

    LightManager* LightManager::Get() {
        return s_Instance;
    }

    void LightManager::Destroy() {
        if (s_Instance) {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }

    Entity LightManager::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    {
        m_DirectionalDirty = true;
        return m_ActiveScene->CreateDirectionalLight(direction, color, intensity);
    }

    Entity LightManager::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius)
    {
        m_PointDirty = true; 
        return m_ActiveScene->CreatePointLight(position, color, intensity, radius);
    }

    Entity LightManager::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity)
    {
        m_SpotDirty = true; 
        return m_ActiveScene->CreateSpotLight(position, direction, cutoff, color, intensity);
    }

    Entity LightManager::CreateShadowCaster(Entity light)
    {
        return m_ActiveScene->CreateShadowCaster(light);
    }

    LightManager::~LightManager() {
        if (m_DirectionalSSBO != 0) {
            glDeleteBuffers(1, &m_DirectionalSSBO);
        }
        if (m_PointSSBO != 0) {
            glDeleteBuffers(1, &m_PointSSBO);
        }
        if (m_SpotSSBO != 0) {
            glDeleteBuffers(1, &m_SpotSSBO);
        }
    }

    void LightManager::CreateSSBOs() {
        glGenBuffers(1, &m_DirectionalSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DirectionalSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DirectionalLightComponent), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DIRECTIONAL_LIGHT_BUFFER_BINDING, m_DirectionalSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &m_PointSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PointLightComponent), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BUFFER_BINDING, m_PointSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &m_SpotSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpotLightComponent), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BUFFER_BINDING, m_SpotSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void LightManager::UpdateSSBOs() {
        auto& registry = m_ActiveScene->GetRegistry();
        UpdateSSBODirect(registry);
        UpdateSSBOSpot(registry);
        UpdateSSBOPoint(registry);
    }

    void LightManager::UpdateSSBOPoint(const entt::registry& registry)
    {
        if (m_PointDirty) {
            auto pointView = registry.view<PointLightComponent>();
            std::vector<PointLightComponent> pointLights;
            for (auto entity : pointView) {
                pointLights.push_back(registry.get<PointLightComponent>(entity));
            }

            size_t bufferSize = std::max(sizeof(PointLightComponent), pointLights.size() * sizeof(PointLightComponent));

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, pointLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            m_PointDirty = false;
        }
    }

    void LightManager::UpdateSSBODirect(const entt::registry& registry)
    {
        if (m_DirectionalDirty) {
            auto directionalView = registry.view<DirectionalLightComponent>();
            std::vector<DirectionalLightComponent> directionalLights;
            for (auto entity : directionalView) {
                directionalLights.push_back(registry.get<DirectionalLightComponent>(entity));
            }
            size_t bufferSize = std::max(sizeof(DirectionalLightComponent), directionalLights.size() * sizeof(DirectionalLightComponent));

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DirectionalSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, directionalLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            m_DirectionalDirty = false;
        }
    }

    void LightManager::UpdateSSBOSpot(const entt::registry& registry)
    {
        if (m_SpotDirty) {
            auto spotView = registry.view<SpotLightComponent>();
            std::vector<SpotLightComponent> spotLights;
            for (auto entity : spotView) {
                spotLights.push_back(registry.get<SpotLightComponent>(entity));
            }

            size_t bufferSize = std::max(sizeof(SpotLightComponent), spotLights.size() * sizeof(SpotLightComponent));

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, spotLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            m_SpotDirty = false;
        }
    }

    void LightManager::BindLightsToShader() {
        UpdateSSBOs();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DIRECTIONAL_LIGHT_BUFFER_BINDING, m_DirectionalSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BUFFER_BINDING, m_PointSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BUFFER_BINDING, m_SpotSSBO);
    }

    void LightManager::SetActiveScene(Scene* activeScene) {
        m_ActiveScene = activeScene;
    }

    void LightManager::UpdateShadowMaps(const ModelPartComponent& partComp, glm::mat4& partTransform) {

        auto& registry = m_ActiveScene->GetRegistry();

        auto pointView = registry.view<PointLightComponent, ShadowCasterComponent>();
        auto dirView = registry.view<DirectionalLightComponent, ShadowCasterComponent>();
        auto spotView = registry.view<SpotLightComponent, ShadowCasterComponent>();

        for (auto entity : pointView) {
            UpdatePointLightShadowMap({ entity, m_ActiveScene }, partComp, partTransform);
        }

        m_ShadowMapShader->Bind();
        for (auto entity : dirView) {
            UpdateDirectionalLightShadowMap({ entity, m_ActiveScene }, partComp, partTransform);
        }

        for (auto entity : spotView) {
            UpdateSpotLightShadowMap({ entity, m_ActiveScene }, partComp, partTransform);
        }
        m_ShadowMapShader->Unbind();

    }

    void LightManager::UpdateDirectionalLightShadowMap(const Entity& entity, const ModelPartComponent& partComp, glm::mat4& partTransform) {
        auto& light = entity.GetComponent<DirectionalLightComponent>();
        auto& shadowCaster = entity.GetComponent<ShadowCasterComponent>();
        // Calculate light space matrix
        glm::mat4 orthoProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowCaster.nearPlane, shadowCaster.farPlane);
        glm::mat4 lightView = glm::lookAt(light.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shadowCaster.lightSpaceMatrix = orthoProjection * lightView;

        m_ShadowMapShader->LoadUniform("lightSpaceMatrix", shadowCaster.lightSpaceMatrix);
        m_ShadowMapShader->LoadUniform("transform", partTransform);

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, shadowCaster.shadowMapResolution, shadowCaster.shadowMapResolution);
        shadowCaster.shadowMap->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        partComp.part->Draw(m_ShadowMapShader);

        shadowCaster.shadowMap->Unbind();
    }

    void LightManager::UpdatePointLightShadowMap(const Entity& entity, const ModelPartComponent& partComp, glm::mat4& partTransform) {

    }

    void LightManager::UpdateSpotLightShadowMap(const Entity& entity, const ModelPartComponent& partComp, glm::mat4& partTransform) {
        auto& light = entity.GetComponent<SpotLightComponent>();
        auto& shadowCaster = entity.GetComponent<ShadowCasterComponent>();
        glm::mat4 lightProjection = glm::perspective(glm::acos(light.cutoff) * 2.0f, 1.0f, shadowCaster.nearPlane, shadowCaster.farPlane);
        glm::mat4 lightView = glm::lookAt(light.position, light.position + light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
        shadowCaster.lightSpaceMatrix = lightProjection * lightView;

        m_ShadowMapShader->LoadUniform("lightSpaceMatrix", shadowCaster.lightSpaceMatrix);
        m_ShadowMapShader->LoadUniform("transform", partTransform);

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, shadowCaster.shadowMapResolution, shadowCaster.shadowMapResolution);
        shadowCaster.shadowMap->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        partComp.part->Draw(m_ShadowMapShader);

        shadowCaster.shadowMap->Unbind();
    }
}

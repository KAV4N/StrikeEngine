#include "strikepch.h"
#include "LightManager.h"
#include "Shader.h"
#include <glad/glad.h>
#include <StrikeEngine/Scene/Components/ShadowCasterComponent.h>

namespace StrikeEngine {

    LightManager* LightManager::s_Instance = nullptr;

    LightManager::LightManager() : m_DirectionalSSBO(0), m_PointSSBO(0), m_SpotSSBO(0), m_IsDirty(false) {
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

    void LightManager::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    {
        m_ActiveScene->CreateDirectionalLight(direction, color, intensity);
        m_IsDirty = true;
    }

    void LightManager::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius)
    {
        m_ActiveScene->CreatePointLight(position, color, intensity, radius);
        m_IsDirty = true;
    }

    void LightManager::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity)
    {
        m_ActiveScene->CreateSpotLight(position, direction, cutoff, color, intensity);
        m_IsDirty = true;
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
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DIRECTIONAL_LIGHT_BUFFER_BINDING, m_DirectionalSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &m_PointSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BUFFER_BINDING, m_PointSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &m_SpotSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BUFFER_BINDING, m_SpotSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


        glGenBuffers(1, &m_ShadowMapSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ShadowMapSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_MAP_BUFFER_BINDING, m_ShadowMapSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

   

    void LightManager::UpdateSSBOs() {
        if (m_IsDirty) {
            auto& registry = m_ActiveScene->GetRegistry();

            // Update Directional Light SSBO
            auto directionalView = registry.view<DirectionalLightComponent>();
            std::vector<DirectionalLightComponent> directionalLights;
            for (auto entity : directionalView) {
                directionalLights.push_back(registry.get<DirectionalLightComponent>(entity));
            }
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DirectionalSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, directionalLights.size() * sizeof(DirectionalLightComponent), directionalLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            // Update Point Light SSBO
            auto pointView = registry.view<PointLightComponent>();
            std::vector<PointLightComponent> pointLights;
            for (auto entity : pointView) {
                pointLights.push_back(registry.get<PointLightComponent>(entity));
            }
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, pointLights.size() * sizeof(PointLightComponent), pointLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            // Update Spot Light SSBO
            auto spotView = registry.view<SpotLightComponent>();
            std::vector<SpotLightComponent> spotLights;
            for (auto entity : spotView) {
                spotLights.push_back(registry.get<SpotLightComponent>(entity));
            }
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, spotLights.size() * sizeof(SpotLightComponent), spotLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            m_IsDirty = false;
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

    void LightManager::UpdateShadowMaps() {
        const std::vector<Entity> shadowCasters = m_ActiveScene->GetShadowCastingLights();
        for (const auto& entity : shadowCasters) {
            auto& shadowCaster = entity.GetComponent<ShadowCasterComponent>();

            if (entity.HasComponent<DirectionalLightComponent>()) {
                UpdateDirectionalLightShadowMap(entity, shadowCaster);
            }
            else if (entity.HasComponent<PointLightComponent>()) {
                UpdatePointLightShadowMap(entity, shadowCaster);
            }
            else if (entity.HasComponent<SpotLightComponent>()) {
                UpdateSpotLightShadowMap(entity, shadowCaster);
            }
        }
    }

    void LightManager::UpdateDirectionalLightShadowMap(const Entity& entity, ShadowCasterComponent& shadowCaster) {
        auto& light = entity.GetComponent<DirectionalLightComponent>();

        // Calculate light space matrix
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowCaster.nearPlane, shadowCaster.farPlane);
        glm::mat4 lightView = glm::lookAt(-light.direction * 10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shadowCaster.lightSpaceMatrix = lightProjection * lightView;

        shadowCaster.shadowMap->Bind();
        glViewport(0, 0, shadowCaster.shadowMapResolution, shadowCaster.shadowMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowCaster.shadowMap->Unbind();
    }

    void LightManager::UpdatePointLightShadowMap(const Entity& entity, ShadowCasterComponent& shadowCaster) {
        auto& light = entity.GetComponent<PointLightComponent>();

        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, shadowCaster.nearPlane, shadowCaster.farPlane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        // Render to cubemap shadow map
        shadowCaster.shadowMap->Bind();
        glViewport(0, 0, shadowCaster.shadowMapResolution, shadowCaster.shadowMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowCaster.shadowMap->Unbind();
    }

    void LightManager::UpdateSpotLightShadowMap(const Entity& entity, ShadowCasterComponent& shadowCaster) {
        auto& light = entity.GetComponent<SpotLightComponent>();

        glm::mat4 lightProjection = glm::perspective(glm::acos(light.cutoff) * 2.0f, 1.0f, shadowCaster.nearPlane, shadowCaster.farPlane);
        glm::mat4 lightView = glm::lookAt(light.position, light.position + light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
        shadowCaster.lightSpaceMatrix = lightProjection * lightView;

        // Render shadow map
        shadowCaster.shadowMap->Bind();
        glViewport(0, 0, shadowCaster.shadowMapResolution, shadowCaster.shadowMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);


        shadowCaster.shadowMap->Unbind();
    }


}

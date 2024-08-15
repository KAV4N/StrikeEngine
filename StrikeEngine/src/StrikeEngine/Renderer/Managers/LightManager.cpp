#include "strikepch.h"
#include "LightManager.h"
#include "StrikeEngine/Renderer/Core/Shader.h" 
#include <glad/glad.h>
#include <StrikeEngine/Scene/Components/ShadowCasterComponent.h>
#include "StrikeEngine/Renderer/Renderer3D/Renderer.h" 
#include "StrikeEngine/Renderer/Core/VisibilityCuller.h"
#include "StrikeEngine/Scene/World.h"

namespace StrikeEngine {

    LightManager* LightManager::s_Instance = nullptr;

    LightManager::LightManager()
        : m_DirectionalSSBO(0), m_PointSSBO(0), m_SpotSSBO(0),
        m_DirectionalDirty(false), m_PointDirty(false), m_SpotDirty(false)   
    {
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
        return World::Get()->GetActiveScene()->CreateDirectionalLight(direction, color, intensity);
    }

    Entity LightManager::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius)
    {
        m_PointDirty = true; 
        return World::Get()->GetActiveScene()->CreatePointLight(position, color, intensity, radius);
    }

    Entity LightManager::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity)
    {
        m_SpotDirty = true; 
        return World::Get()->GetActiveScene()->CreateSpotLight(position, direction, cutoff, color, intensity);
    }

    Entity LightManager::CreateShadowCaster(Entity light)
    {
        return World::Get()->GetActiveScene()->CreateShadowCaster(light);
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
        auto& registry = World::Get()->GetActiveScene()->GetRegistry();
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

    void LightManager::BindLights() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DIRECTIONAL_LIGHT_BUFFER_BINDING, m_DirectionalSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BUFFER_BINDING, m_PointSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BUFFER_BINDING, m_SpotSSBO);
    }
}

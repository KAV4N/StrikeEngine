#include "strikepch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "LightManager.h"
#include "StrikeEngine/Graphics/Core/Shader.h" 
#include <glad/glad.h>
#include <StrikeEngine/Scene/Components/ShadowCasterComponent.h>
#include "StrikeEngine/Graphics/Renderer/Renderer.h" 
#include "StrikeEngine/Graphics/Core/VisibilityCuller.h"
#include "StrikeEngine/Scene/World.h"
#include <glm/gtx/euler_angles.hpp>

namespace StrikeEngine {

    LightManager* LightManager::s_Instance = nullptr;

    LightManager::LightManager()
        : m_DirectionalSSBO(0), m_PointSSBO(0), m_SpotSSBO(0),
        m_DirectionalCount(0), m_SpotCount(0), m_PointCount(0),
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

    void LightManager::SetPointLightsDirty() {
        s_Instance->m_PointDirty = true;
    }
    void LightManager::SetSpotLightsDirty() {
        s_Instance->m_SpotDirty = true;
    }
    void LightManager::SetDirectionalLightsDirty() {
        s_Instance->m_DirectionalDirty = true;
    }

    Entity LightManager::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    {
        m_DirectionalDirty = true;
        m_DirectionalCount++;
        return World::Get()->GetActiveScene()->CreateDirectionalLight(direction, color, intensity);
    }

    Entity LightManager::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius)
    {
        m_PointDirty = true; 
        m_PointCount++;
        return World::Get()->GetActiveScene()->CreatePointLight(position, color, intensity, radius);
    }

    Entity LightManager::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity)
    {
        m_SpotDirty = true;
        m_SpotCount++;
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

        glGenBuffers(1, &m_PointSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PointLightComponent), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BUFFER_BINDING, m_PointSSBO);

        glGenBuffers(1, &m_SpotSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpotLightData), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BUFFER_BINDING, m_SpotSSBO);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void LightManager::UpdateSSBOs() {
        auto& registry = World::Get()->GetActiveScene()->GetRegistry();
        if (m_PointDirty || m_SpotDirty || m_DirectionalDirty) {
            UpdateSSBODirect(registry);
            UpdateSSBOSpot(registry);
            UpdateSSBOPoint(registry);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

    }

    void LightManager::UpdateSSBOPoint(const entt::registry& registry)
    {
        if (m_PointDirty) {
            auto pointView = registry.view<PointLightComponent>();
            std::vector<PointLightData> pointLights;
            for (auto entity : pointView) {
                PointLightData pointData;
                auto& lightComp = registry.get<PointLightComponent>(entity);
                auto& transformComp = registry.get<TransformComponent>(entity);

                pointData.color = lightComp.color;
                pointData.intensity = lightComp.intensity;
                pointData.radius = lightComp.radius;
                pointData.position = transformComp.position;
 

                pointLights.push_back(pointData);
            }

            size_t bufferSize = m_PointCount * sizeof(PointLightData);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, pointLights.data(), GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BUFFER_BINDING, m_PointSSBO);
            m_PointDirty = false;
        }
    }

    void LightManager::UpdateSSBODirect(const entt::registry& registry)
    {
        if (m_DirectionalDirty) {
            auto directionalView = registry.view<DirectionalLightComponent>();
            std::vector<DirectionalLightData> directionalLights;
            for (auto entity : directionalView) {
                DirectionalLightData dirData;
                auto& lightComp = registry.get<DirectionalLightComponent>(entity);
                auto& transformComp = registry.get<TransformComponent>(entity);

                glm::mat4 rotationMatrix = glm::eulerAngleYXZ(
                    glm::radians(transformComp.rotation.y),
                    glm::radians(transformComp.rotation.x),
                    glm::radians(transformComp.rotation.z)
                );

                dirData.color = lightComp.color;
                dirData.intensity = lightComp.intensity;
                dirData.direction = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));

                directionalLights.push_back(dirData);
            }
            size_t bufferSize = m_DirectionalCount * sizeof(DirectionalLightData);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DirectionalSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, directionalLights.data(), GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DIRECTIONAL_LIGHT_BUFFER_BINDING, m_DirectionalSSBO);
            m_DirectionalDirty = false;
        }
    }

    void LightManager::UpdateSSBOSpot(const entt::registry& registry)
    {
        if (m_SpotDirty) {
            auto spotView = registry.view<SpotLightComponent, TransformComponent>();
            std::vector<SpotLightData> spotLights;
            for (auto entity : spotView) {
                SpotLightData spotData;
                auto& lightComp = registry.get<SpotLightComponent>(entity);
                auto& transformComp = registry.get<TransformComponent>(entity);
                
                glm::mat4 rotationMatrix = glm::eulerAngleYXZ(
                    glm::radians(transformComp.rotation.y),
                    glm::radians(transformComp.rotation.x),
                    glm::radians(transformComp.rotation.z)
                );

                spotData.color = lightComp.color;
                spotData.intensity = lightComp.intensity;
                spotData.cutoff = lightComp.cutoff;

                spotData.position = transformComp.position;
                spotData.direction = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));


                spotLights.push_back(spotData);

            }

            size_t bufferSize = m_SpotCount * sizeof(SpotLightData);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, spotLights.data(), GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BUFFER_BINDING, m_SpotSSBO);
            m_SpotDirty = false;
        }
    }



    void LightManager::BindLights() {
      
        
     
    }
}

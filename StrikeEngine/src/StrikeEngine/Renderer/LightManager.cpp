#include "strikepch.h"
#include "LightManager.h"
#include "Shader.h"
#include <glad/glad.h>

namespace StrikeEngine {

    LightManager* LightManager::s_Instance = nullptr;

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

    LightManager::LightManager() : m_DirectionalSSBO(0), m_PointSSBO(0), m_SpotSSBO(0), m_IsDirty(false) {
        CreateSSBOs();
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

    }

    void LightManager::AddDirectionalLight(const DirectionalLight& light) {
        m_ActiveScene->AddDirectionalLight(light);
        m_IsDirty = true;
    }

    void LightManager::RemoveDirectionalLight(size_t index) {
        m_ActiveScene->RemoveDirectionalLight(index);
        m_IsDirty = true;
    }

    void LightManager::UpdateDirectionalLight(size_t index, const DirectionalLight& light) {
        m_ActiveScene->UpdateDirectionalLight(index, light);
        m_IsDirty = true;
    }

    void LightManager::AddPointLight(const PointLight& light) {
        m_ActiveScene->AddPointLight(light);
        m_IsDirty = true;
    }

    void LightManager::RemovePointLight(size_t index) {
        m_ActiveScene->RemovePointLight(index);
        m_IsDirty = true;
        
    }

    void LightManager::UpdatePointLight(size_t index, const PointLight& light) {
        m_ActiveScene->UpdatePointLight(index, light);
        m_IsDirty = true;
    }

    void LightManager::AddSpotLight(const SpotLight& light) {
        m_ActiveScene->AddSpotLight(light);
        m_IsDirty = true;
    }

    void LightManager::RemoveSpotLight(size_t index) {
        m_ActiveScene->RemoveSpotLight(index);
        m_IsDirty = true;
    }

    void LightManager::UpdateSpotLight(size_t index, const SpotLight& light) {
        m_ActiveScene->UpdateSpotLight(index, light);
        m_IsDirty = true;
    }

    void LightManager::ClearDirectionalLights() {
        m_ActiveScene->ClearDirectionalLights();
        m_IsDirty = true;
    }

    void LightManager::ClearPointLights() {
        m_ActiveScene->ClearPointLights();
        m_IsDirty = true;
    }

    void LightManager::ClearSpotLights() {
        m_ActiveScene->ClearSpotLights();
        m_IsDirty = true;
    }

    void LightManager::UpdateSSBOs() {
        if (m_IsDirty) {
            std::vector<DirectionalLight> directionalLights = m_ActiveScene->GetDirectionalLights();
            std::vector<PointLight> pointLights = m_ActiveScene->GetPointLights();
            std::vector<SpotLight> spotLights = m_ActiveScene->GetSpotLights();
            // Update Directional Light SSBO
            
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DirectionalSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, directionalLights.size() * sizeof(DirectionalLight), directionalLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            // Update Point Light SSBO
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PointSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, pointLights.size() * sizeof(PointLight), pointLights.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


            // Update Spot Light SSBO
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpotSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, spotLights.size() * sizeof(SpotLight), spotLights.data(), GL_DYNAMIC_DRAW);
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

}

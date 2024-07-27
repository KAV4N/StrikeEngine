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

    LightManager::LightManager() : m_SSBO(0), m_IsDirty(false) {
        CreateSSBO();
    }

    LightManager::~LightManager() {
        if (m_SSBO != 0) {
            glDeleteBuffers(1, &m_SSBO);
        }
    }

    void LightManager::CreateSSBO() {
        glGenBuffers(1, &m_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIGHT_BUFFER_BINDING, m_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void LightManager::AddLight(const Light& light) {
        m_ActiveScene->AddLight(light);
        m_IsDirty = true;
    }

    void LightManager::RemoveLight(size_t index) {
        if (index < m_ActiveScene->GetLights().size()) {
            m_ActiveScene->RemoveLight(index);
            m_IsDirty = true;
        }
    }

    void LightManager::UpdateLight(size_t index, const Light& light) {
        if (index < m_ActiveScene->GetLights().size()) {
            m_ActiveScene->UpdateLight(index, light);
            m_IsDirty = true;
        }
    }

    void LightManager::Clear() {
        m_ActiveScene->ClearLights();
        m_IsDirty = true;
    }

    void LightManager::UpdateSSBO() {
        if (m_IsDirty) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, m_ActiveScene->GetLights().size() * sizeof(Light), m_ActiveScene->GetLights().data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            m_IsDirty = false;
        }
    }

    void LightManager::BindLightsToShader() {
        UpdateSSBO();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIGHT_BUFFER_BINDING, m_SSBO);
    }

    void LightManager::SetActiveScene(Scene* activeScene)
    {
        m_ActiveScene = activeScene;
    }

} 
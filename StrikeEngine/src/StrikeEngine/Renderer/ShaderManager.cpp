#include "strikepch.h"
#include "ShaderManager.h"
#include <glad/glad.h>

namespace StrikeEngine {

    ShaderManager* ShaderManager::s_Instance = nullptr;

    ShaderManager::ShaderManager()
        : m_ProgramID(glCreateProgram()) {}

    void ShaderManager::Create() {
        if (!s_Instance) {
            s_Instance = new ShaderManager();
        }
    }

    ShaderManager* ShaderManager::Get() {
        return s_Instance;
    }

    void ShaderManager::Shutdown() {
        if (s_Instance) {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }

    Shader* ShaderManager::LoadShaderFromFile(const std::string& name, const std::string& shaderPath, const unsigned int shaderType) {
        if (m_Shaders.find(name) != m_Shaders.end()) {
            // std::cerr << "Shader with name '" << name << "' already exists." << std::endl;
            return nullptr;
        }

        Shader* shader = new Shader(shaderType);
        if (!shader->LoadFromFile(shaderPath, m_ProgramID)) {
            delete shader;
            return nullptr;
        }
        if (!shader->Compile()) {
            delete shader;
            return nullptr;
        }

        m_Shaders[name] = shader;

        return shader;
    }

    Shader* ShaderManager::GetShader(const std::string& name) {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            return it->second;
        }
        return nullptr;
    }

    void ShaderManager::Bind() {
        for (auto& pair : m_Shaders) {
            Shader* shader = pair.second;
            if (!shader->LinkProgram()) {
                STRIKE_CORE_ERROR("Failed to link shader program");
                return;
            }
        }
        glUseProgram(m_ProgramID);
    }

    ShaderManager::~ShaderManager() {
        for (auto& pair : m_Shaders) {
            delete pair.second;
        }
        m_Shaders.clear();
    }
}

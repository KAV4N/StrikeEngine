#pragma once

#include "strikepch.h"
#include "Shader.h"
#include "ShaderManager.h"

namespace StrikeEngine {

   ShaderManager* ShaderManager::s_Instance = nullptr;

    ShaderManager* ShaderManager::Get() {
        
        return s_Instance;
    }

    void ShaderManager::Create()
    {
        if (!s_Instance) {
            s_Instance = new ShaderManager();
        }
    }

    Shader* ShaderManager::LoadShader(const std::string& name, const std::string& filepath) {
        Shader* shader = new Shader(filepath);
        m_Shaders[name] = shader;
        return shader;
    }

    Shader* ShaderManager::LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        Shader* shader = new Shader(vertexSrc, fragmentSrc);
        m_Shaders[name] = shader;
        return shader;
    }

    Shader* ShaderManager::GetShader(const std::string& name) const {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            return it->second;
        }
        STRIKE_CORE_ERROR("Shader not found: {0}", name);
        return nullptr;
    }

}

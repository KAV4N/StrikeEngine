#pragma once

#include "strikepch.h"
#include "Shader.h"
#include <filesystem>
#include "ShaderManager.h"

namespace StrikeEngine {

    ShaderManager* ShaderManager::s_Instance = nullptr;

    ShaderManager* ShaderManager::Get() {
        return s_Instance;
    }

    void ShaderManager::Create() {
        if (!s_Instance) {
            s_Instance = new ShaderManager();
        }
    }
    Shader* ShaderManager::LoadShader(const std::string& name, const std::string& filepath) {
        Shader* shader = new Shader(filepath);
        m_Shaders[name] = shader;
        return shader;
    }

    void ShaderManager::LoadAllShaders() {
        for (const auto& entry : std::filesystem::directory_iterator(SHADER_DIRECTORY)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                std::string filename = entry.path().stem().string(); 
                LoadShader(filename, filepath);
            }
        }
    }

    std::vector<std::string> ShaderManager::GetShaderNames() {
        std::vector<std::string> shaderNames;
        for (const auto& shaderPair : m_Shaders) {
            shaderNames.push_back(shaderPair.first);
        }
        return shaderNames;
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
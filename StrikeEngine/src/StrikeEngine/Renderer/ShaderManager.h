#pragma once

#define DEFAULT_SHADER "DefaultShader"
#define SHADER_DIRECTORY "assets/shaders"

#include <string>
#include <unordered_map>
#include <memory>
#include "Shader.h"

namespace StrikeEngine {

    class ShaderManager {
    public:
        static ShaderManager* Get();
        static void Create();

        Shader* LoadShader(const std::string& name, const std::string& filepath);
        Shader* LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        Shader* GetShader(const std::string& name) const;
        void LoadAllShaders();
        std::vector<std::string> GetShaderNames();

        inline std::string GetDefaultShader() const { return m_DefaultShader; }

    private:
        ShaderManager()
        {
            LoadAllShaders();
        }

        ~ShaderManager() = default;
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;
        std::string m_DefaultShader;
        std::unordered_map<std::string, Shader*> m_Shaders;
        static ShaderManager* s_Instance;
    };


}

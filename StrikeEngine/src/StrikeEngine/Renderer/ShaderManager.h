#pragma once

#include <string>
#include <unordered_map>
#include "Shader.h"

namespace StrikeEngine {

    class ShaderManager {
    public:
        static void Create();
        static ShaderManager* Get();
        static void Shutdown();

        Shader* LoadShaderFromFile(const std::string& name, const std::string& shaderPath, const unsigned int shaderType);
        Shader* GetShader(const std::string& name);
        void Bind();

    private:
        ShaderManager();
        ~ShaderManager();

        static ShaderManager* s_Instance;
        unsigned int m_ProgramID;
        std::unordered_map<std::string, Shader*> m_Shaders;
    };

}

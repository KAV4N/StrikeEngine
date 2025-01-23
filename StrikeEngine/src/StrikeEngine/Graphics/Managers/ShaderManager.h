#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <glad/glad.h>

namespace StrikeEngine {
    class Shader;

    class ShaderManager {
    public:
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;

        static ShaderManager* Create();
        static ShaderManager* Get();

        std::shared_ptr<Shader> LoadShader(const std::string& filepath, const std::string& name);
        std::shared_ptr<Shader> GetShader(const std::string& name);
        void RemoveShader(const std::string& name);
        bool HasShader(const std::string& name) const;
        void Clear();

    private:
        ShaderManager() = default;
        ~ShaderManager();

        std::string ReadFile(const std::string& filepath);
        GLenum ShaderTypeFromString(const std::string& type);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        GLuint CompileShaders(const std::unordered_map<GLenum, std::string>& shaderSources);

        static ShaderManager* s_Instance;
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
    };
}

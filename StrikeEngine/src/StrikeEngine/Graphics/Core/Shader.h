#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>

namespace StrikeEngine {

    class Shader {
    public:
        Shader(const std::string& filepath, std::string name);
        ~Shader();

        void Bind();
        void Unbind();

        void LoadUniform(const std::string& name, const std::vector<glm::vec3>& values);
        void LoadUniform(const std::string& name, const std::vector<glm::mat4>& values);

        void LoadUniform(const std::string& name, const glm::vec2& value);

        void LoadUniform(const std::string& name, const glm::vec3& value);
        void LoadUniform(const std::string& name, const glm::mat4& value);
        void LoadUniform(const std::string& name, float value);
        void LoadUniform(const std::string& name, int value);

        inline std::string& GetName() { return m_Name; }
        inline std::string& GetFilePath() { return m_FilePath; }
    private:
        std::string ReadFile(const std::string& filepath);
        void ParseUniforms(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
        void SetUniformsFromJSON(const nlohmann::json& json);

        GLenum ShaderTypeFromString(const std::string& type);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

        std::string m_Name;
        std::string m_FilePath;

        GLuint m_ProgramID;
        std::unordered_map<std::string, GLint> m_UniformLocations;
        std::unordered_map<std::string, std::string> m_UniformTypes;
    };
}

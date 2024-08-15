#include "strikepch.h"
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <nlohmann/json.hpp> 

namespace StrikeEngine {

    Shader::Shader(const std::string& filepath, std::string name)
        : m_Name(name), m_FilePath(filepath)
    {
        std::string source = ReadFile(filepath);

        size_t end = source.find("#type");
        if (end != std::string::npos) {
            std::string newSource = source.substr(end);
            auto shaderSources = PreProcess(newSource);
            Compile(shaderSources);
            ParseUniforms(source);
        }
    }

    Shader::~Shader() {
        glDeleteProgram(m_ProgramID);
    }

    void Shader::Bind() {
        glUseProgram(m_ProgramID);
    }

    void Shader::Unbind() {
        glUseProgram(0);
    }

    std::string Shader::ReadFile(const std::string& filepath) {
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void Shader::ParseUniforms(const std::string& source) {
        std::string jsonStr;
        size_t pos = source.find("#RootSignature");
        if (pos != std::string::npos) {
            pos = source.find("{", pos);
            size_t endPos = source.find("#end", pos);
            if (pos != std::string::npos && endPos != std::string::npos) {
                jsonStr = source.substr(pos, endPos - pos);
            }
        }

        if (!jsonStr.empty()) {
            try {
                auto json = nlohmann::json::parse(jsonStr);
                SetUniformsFromJSON(json["RootSignature"]);
            }
            catch (const nlohmann::json::exception& e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
                return;
            }
        }
    }

    void Shader::SetUniformsFromJSON(const nlohmann::json& json) {
        for (const auto& item : json) {
            std::string name = item["name"];
            std::string type = item["type"];
            m_UniformTypes[name] = type;
            m_UniformLocations[name] = glGetUniformLocation(m_ProgramID, name.c_str());
        }
    }

    void Shader::LoadUniform(const std::string& name, const std::vector<glm::vec3>& values) {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            glUniform3fv(it->second, values.size(), glm::value_ptr(values[0]));
        }
    }

    void Shader::LoadUniform(const std::string& name, const std::vector<glm::mat4>& values) {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            glUniformMatrix4fv(it->second, values.size(), GL_FALSE, glm::value_ptr(values[0]));
        }
    }

    void Shader::LoadUniform(const std::string& name, const glm::vec3& value) {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            glUniform3f(it->second, value.x, value.y, value.z);
        }
    }

    void Shader::LoadUniform(const std::string& name, const glm::mat4& value) {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            glUniformMatrix4fv(it->second, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void Shader::LoadUniform(const std::string& name, float value) {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            glUniform1f(it->second, value);
        }
    }

    void Shader::LoadUniform(const std::string& name, int value) {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            glUniform1i(it->second, value);
        }
    }

    GLenum Shader::ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        else if (type == "fragment")
            return GL_FRAGMENT_SHADER;

        assert(false && "Unknown shader type!");
        return 0;
    }

    std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source) {
        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            size_t begin = pos + strlen(typeToken) + 1;
            std::string type = source.substr(begin, eol - begin);

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);

            shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
        m_ProgramID = glCreateProgram();
        std::vector<GLuint> shaderIDs;

        for (auto& kv : shaderSources) {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLuint shader = glCreateShader(type);
            const char* src = source.c_str();
            glShaderSource(shader, 1, &src, 0);
            glCompileShader(shader);

            GLint isCompiled;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<char> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

                glDeleteShader(shader);

                std::cerr << "Shader compilation error: " << infoLog.data() << std::endl;
                return;
            }

            glAttachShader(m_ProgramID, shader);
            shaderIDs.push_back(shader);
        }

        glLinkProgram(m_ProgramID);

        GLint isLinked;
        glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength;
            glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> infoLog(maxLength);
            glGetProgramInfoLog(m_ProgramID, maxLength, &maxLength, infoLog.data());

            glDeleteProgram(m_ProgramID);

            for (auto id : shaderIDs) {
                glDeleteShader(id);
            }

            std::cerr << "Shader linking error: " << infoLog.data() << std::endl;
            return;
        }

        for (auto id : shaderIDs) {
            glDetachShader(m_ProgramID, id);
            glDeleteShader(id);
        }
    }
}

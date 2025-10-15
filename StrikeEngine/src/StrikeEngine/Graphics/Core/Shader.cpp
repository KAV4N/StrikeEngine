#include "strikepch.h"
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

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

    void Shader::LoadUniform(const std::string& name, const std::vector<glm::vec3>& values) {
        glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), values.size(), glm::value_ptr(values[0]));
    }

    void Shader::LoadUniform(const std::string& name, const std::vector<glm::mat4>& values) {
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), values.size(), GL_FALSE, glm::value_ptr(values[0]));
    }
    
    void Shader::LoadUniform(const std::string& name, const glm::vec2& value) {
        glUniform2f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y);
    }

    void Shader::LoadUniform(const std::string& name, const glm::vec3& value) {
        glUniform3f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z);
    }


    void Shader::LoadUniform(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        
    }

    void Shader::LoadUniform(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value);
    }

    void Shader::LoadUniform(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value);
    }

    GLenum Shader::ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        else if (type == "fragment")
            return GL_FRAGMENT_SHADER;
        else if (type == "geometry")
            return GL_GEOMETRY_SHADER;

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

            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos)
                ? source.substr(nextLinePos)
                : source.substr(nextLinePos, pos - nextLinePos);
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

                std::cerr << "Shader compilation error for "
                    << (type == GL_VERTEX_SHADER ? "vertex" :
                        type == GL_FRAGMENT_SHADER ? "fragment" : "geometry")
                    << " shader: " << infoLog.data() << std::endl;
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
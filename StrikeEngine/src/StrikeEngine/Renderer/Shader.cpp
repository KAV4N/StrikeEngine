#include "strikepch.h"
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

namespace StrikeEngine {

    Shader::Shader(const std::string& filepath) {
        std::string source = ReadFile(filepath);
        auto shaderSources = PreProcess(source);
        Compile(shaderSources);
        GetAllUniformLocations();
    }

    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
        GetAllUniformLocations();
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

    void Shader::GetAllUniformLocations() {
        m_TransformationMatrix = GetUniformLocation("transform");
        m_ProjectionMatrix = GetUniformLocation("projection");
        m_ViewMatrix = GetUniformLocation("view");
        m_ViewPosition = GetUniformLocation("viewPosition");
        
    }

    std::string Shader::ReadFile(const std::string& filepath) {
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
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

    GLenum Shader::ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        else if (type == "fragment")
            return GL_FRAGMENT_SHADER;

        assert(false && "Unknown shader type!");
        return 0;
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

#include "strikepch.h"
#include "ShaderManager.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

namespace StrikeEngine {
    ShaderManager* ShaderManager::s_Instance = nullptr;

    ShaderManager* ShaderManager::Create() {
        if (s_Instance == nullptr) {
            s_Instance = new ShaderManager();
        }
        return s_Instance;
    }

    ShaderManager* ShaderManager::Get() {
        return s_Instance;
    }

    ShaderManager::~ShaderManager() {
        Clear();
        s_Instance = nullptr;
    }

    std::shared_ptr<Shader> ShaderManager::LoadShader(const std::string& filepath, const std::string& name) {
        if (HasShader(name)) {
            std::cerr << "Shader with name '" << name << "' already exists. Returning existing shader." << std::endl;
            return GetShader(name);
        }

        try {
            std::string source = ReadFile(filepath);
            auto shaderSources = PreProcess(source);
            GLuint programId = CompileShaders(shaderSources);
            auto shader = std::make_shared<Shader>(programId, name, filepath);
            m_Shaders[name] = shader;
            return shader;
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to load shader '" << name << "': " << e.what() << std::endl;
            return nullptr;
        }
    }

    std::shared_ptr<Shader> ShaderManager::GetShader(const std::string& name) {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            return it->second;
        }

        std::cerr << "Shader with name '" << name << "' not found." << std::endl;
        return nullptr;
    }

    void ShaderManager::RemoveShader(const std::string& name) {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            m_Shaders.erase(it);
        }
        else {
            std::cerr << "Shader with name '" << name << "' not found." << std::endl;
        }
    }

    bool ShaderManager::HasShader(const std::string& name) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }

    void ShaderManager::Clear() {
        m_Shaders.clear();
    }

    std::string ShaderManager::ReadFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open shader file: " + filepath);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    GLenum ShaderManager::ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        else if (type == "fragment")
            return GL_FRAGMENT_SHADER;
        else if (type == "geometry")
            return GL_GEOMETRY_SHADER;

        throw std::runtime_error("Unknown shader type: " + type);
    }

    std::unordered_map<GLenum, std::string> ShaderManager::PreProcess(const std::string& source) {
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

    GLuint ShaderManager::CompileShaders(const std::unordered_map<GLenum, std::string>& shaderSources) {
        GLuint programId = glCreateProgram();
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

                std::string shaderTypeStr =
                    type == GL_VERTEX_SHADER ? "vertex" :
                    type == GL_FRAGMENT_SHADER ? "fragment" :
                    type == GL_GEOMETRY_SHADER ? "geometry" : "unknown";

                throw std::runtime_error("Shader compilation error for " +
                    shaderTypeStr + " shader: " + std::string(infoLog.data()));
            }

            glAttachShader(programId, shader);
            shaderIDs.push_back(shader);
        }


        glLinkProgram(programId);

        GLint isLinked;
        glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> infoLog(maxLength);
            glGetProgramInfoLog(programId, maxLength, &maxLength, infoLog.data());

            glDeleteProgram(programId);
            for (auto id : shaderIDs) {
                glDeleteShader(id);
            }

            throw std::runtime_error("Shader linking error: " + std::string(infoLog.data()));
        }

        for (auto id : shaderIDs) {
            glDetachShader(programId, id);
            glDeleteShader(id);
        }

        return programId;
    }
}

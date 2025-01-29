#include "strikepch.h"
#include "ShaderManager.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <vector>

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
            std::cerr << "Shader with name '" << name << "' already exists. Returning existing shader.\n";
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
            std::cerr << "Failed to load shader '" << name << "': " << e.what() << "\n";
            return nullptr;
        }
    }

    std::shared_ptr<Shader> ShaderManager::GetShader(const std::string& name) const {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            return it->second;
        }
        std::cerr << "Shader with name '" << name << "' not found.\n";
        return nullptr;
    }

    void ShaderManager::RemoveShader(const std::string& name) {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            m_Shaders.erase(it);
        }
        else {
            std::cerr << "Shader with name '" << name << "' not found.\n";
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
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;
        if (type == "geometry")
            return GL_GEOMETRY_SHADER;

        throw std::runtime_error("Unknown shader type: " + type);
    }

    std::unordered_map<GLenum, std::string> ShaderManager::PreProcess(const std::string& source) {
        std::unordered_map<GLenum, std::string> shaderSources;
        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);

        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            if (eol == std::string::npos)
                throw std::runtime_error("Syntax error");

            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] = source.substr(
                nextLinePos,
                pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)
            );
        }

        return shaderSources;
    }

    GLuint ShaderManager::CompileShaders(const std::unordered_map<GLenum, std::string>& shaderSources) {
        GLuint program = glCreateProgram();
        std::vector<GLuint> shaderIDs;

        for (auto& kv : shaderSources) {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLuint shader = glCreateShader(type);
            const char* sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                glDeleteShader(shader);

                std::string typeStr;
                switch (type) {
                case GL_VERTEX_SHADER: typeStr = "vertex"; break;
                case GL_FRAGMENT_SHADER: typeStr = "fragment"; break;
                case GL_GEOMETRY_SHADER: typeStr = "geometry"; break;
                default: typeStr = "unknown";
                }

                throw std::runtime_error("Shader compilation failed (" + typeStr + "):\n" + infoLog.data());
            }

            glAttachShader(program, shader);
            shaderIDs.push_back(shader);
        }

        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            for (auto id : shaderIDs)
                glDeleteShader(id);
            glDeleteProgram(program);

            throw std::runtime_error("Shader linking failed:\n" + std::string(infoLog.data()));
        }

        for (auto id : shaderIDs) {
            glDetachShader(program, id);
            glDeleteShader(id);
        }

        return program;
    }

}
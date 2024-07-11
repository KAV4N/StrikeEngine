#include "strikepch.h"
#include "Shader.h"
#include <fstream>
#include "glad/glad.h"

namespace StrikeEngine {

    Shader::Shader(unsigned int type)
        : m_Type(type), m_ShaderID(0), m_ProgramID(0) {}

    Shader::~Shader() {
        if (m_ShaderID) {
            glDeleteShader(m_ShaderID);
        }
    }

    bool Shader::LoadFromFile(const std::string& filePath, unsigned int& programID) {
        m_ProgramID = programID;
        std::ifstream shaderFile(filePath);
        if (!shaderFile.is_open()) {
            //std::cerr << "Failed to open shader file: " << filePath << std::endl;
            return false;
        }

        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        m_SourceCode = shaderStream.str();
        return true;
    }

    bool Shader::Compile() {
        int success = 0;
        const char* shaderSource = m_SourceCode.c_str();

        m_ShaderID = glCreateShader(m_Type);

        glShaderSource(m_ShaderID, 1, &shaderSource, nullptr);

        glCompileShader(m_ShaderID);

        glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(m_ShaderID, 512, nullptr, infoLog);
            STRIKE_CORE_ERROR(infoLog, "ERROR: Shader compilation failed");
            return false;
        }

        glAttachShader(m_ProgramID, m_ShaderID);
        return true;
    }

    bool Shader::LinkProgram() {
        int success = 0;

        glLinkProgram(m_ProgramID);
        glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
            STRIKE_CORE_ERROR(infoLog, "ERROR: Program linking failed");
            return false;
        }

        glDetachShader(m_ProgramID, m_ShaderID);
        return true;
    }
}

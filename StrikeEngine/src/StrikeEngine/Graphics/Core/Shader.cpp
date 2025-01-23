#include "strikepch.h"
#include "Shader.h"
#include <iostream>

namespace StrikeEngine {
    Shader::Shader(GLuint programId, const std::string& name, const std::string& filepath)
        : m_ProgramID(programId), m_Name(name), m_FilePath(filepath)
    {
    }

    Shader::~Shader() {
        if (m_ProgramID != 0) {
            glDeleteProgram(m_ProgramID);
        }
    }

    void Shader::Bind() {
        glUseProgram(m_ProgramID);
    }

    void Shader::Unbind() {
        glUseProgram(0);
    }

    void Shader::SetInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value);
    }

    void Shader::SetFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value);
    }

    void Shader::SetBool(const std::string& name, bool value) {
        glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), static_cast<int>(value));
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
        glUniform2f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z);
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
        glUniform4f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z, value.w);
    }

    void Shader::SetMat2(const std::string& name, const glm::mat2& value) {
        glUniformMatrix2fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
        glUniformMatrix3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetIVec2(const std::string& name, const glm::ivec2& value) {
        glUniform2i(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y);
    }

    void Shader::SetIVec3(const std::string& name, const glm::ivec3& value) {
        glUniform3i(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z);
    }

    void Shader::SetIVec4(const std::string& name, const glm::ivec4& value) {
        glUniform4i(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z, value.w);
    }

    void Shader::SetBVec2(const std::string& name, const glm::bvec2& value) {
        glUniform2i(glGetUniformLocation(m_ProgramID, name.c_str()),
            static_cast<int>(value.x), static_cast<int>(value.y));
    }

    void Shader::SetBVec3(const std::string& name, const glm::bvec3& value) {
        glUniform3i(glGetUniformLocation(m_ProgramID, name.c_str()),
            static_cast<int>(value.x), static_cast<int>(value.y), static_cast<int>(value.z));
    }

    void Shader::SetBVec4(const std::string& name, const glm::bvec4& value) {
        glUniform4i(glGetUniformLocation(m_ProgramID, name.c_str()),
            static_cast<int>(value.x), static_cast<int>(value.y),
            static_cast<int>(value.z), static_cast<int>(value.w));
    }

    void Shader::SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values) {
        glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), values.size(), glm::value_ptr(values[0]));
    }

    void Shader::SetMat4Array(const std::string& name, const std::vector<glm::mat4>& values) {
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), values.size(), GL_FALSE, glm::value_ptr(values[0]));
    }

    void Shader::SetTexture(const std::string& name, GLuint textureId) {
        glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), textureId);
    }

    void Shader::SetStorageBuffer(const std::string& name, GLuint bufferId, GLuint bindingPoint) {
        GLuint blockIndex = glGetProgramResourceIndex(m_ProgramID, GL_SHADER_STORAGE_BLOCK, name.c_str());
        if (blockIndex != GL_INVALID_INDEX) {
            glShaderStorageBlockBinding(m_ProgramID, blockIndex, bindingPoint);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferId);
        }
    }
}

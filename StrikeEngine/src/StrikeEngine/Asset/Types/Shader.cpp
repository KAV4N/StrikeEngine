#include "Shader.h"
#include "glad/glad.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {

    Shader::Shader(const std::string& id, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::string& name)
        : Asset(id, vertexPath, name), mFragmentPath(fragmentPath), mRendererID(0) {
    }

    Shader::~Shader() {
        if (mRendererID != 0) {
            glDeleteProgram(mRendererID);
        }
    }

    void Shader::bind() const {
        if (mRendererID != 0) {
            glUseProgram(mRendererID);
        }
    }

    void Shader::unbind() const {
        glUseProgram(0);
    }

    void Shader::postLoad() {
        if (mVertexSource.empty() || mFragmentSource.empty()) {
            std::cerr << "Shader sources are empty for shader: " << getId() << std::endl;
            return;
        }

        try {
            GLuint vertexShader = compileShader(GL_VERTEX_SHADER, mVertexSource);
            GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, mFragmentSource);

            mRendererID = glCreateProgram();
            glAttachShader(mRendererID, vertexShader);
            glAttachShader(mRendererID, fragmentShader);

            linkProgram();

            glDetachShader(mRendererID, vertexShader);
            glDetachShader(mRendererID, fragmentShader);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create OpenGL resources for shader " << getId() << ": " << e.what() << std::endl;
            if (mRendererID != 0) {
                glDeleteProgram(mRendererID);
                mRendererID = 0;
            }
        }
    }

    GLuint Shader::compileShader(GLenum type, const std::string& source) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, '\0');
            glGetShaderInfoLog(shader, length, nullptr, log.data());

            const std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            const std::string errorMsg = "Shader compilation error (" + shaderType + ") for " + getId() + ":\n" + log;

            glDeleteShader(shader);
            throw std::runtime_error(errorMsg);
        }

        return shader;
    }

    void Shader::linkProgram() {
        glLinkProgram(mRendererID);

        GLint success;
        glGetProgramiv(mRendererID, GL_LINK_STATUS, &success);
        if (!success) {
            GLint length;
            glGetProgramiv(mRendererID, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, '\0');
            glGetProgramInfoLog(mRendererID, length, nullptr, log.data());

            const std::string errorMsg = "Shader linking error for " + getId() + ":\n" + log;
            throw std::runtime_error(errorMsg);
        }
    }

    GLint Shader::getUniformLocation(const std::string& name) {
        auto it = mUniformLocationCache.find(name);
        if (it != mUniformLocationCache.end()) {
            return it->second;
        }

        GLint location = glGetUniformLocation(mRendererID, name.c_str());
        mUniformLocationCache[name] = location;
        return location;
    }

    void Shader::setInt(const std::string& name, int value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1i(location, value);
        }
    }

    void Shader::setIntArray(const std::string& name, int* values, uint32_t count) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1iv(location, count, values);
        }
    }

    void Shader::setFloat(const std::string& name, float value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1f(location, value);
        }
    }

    void Shader::setVec2(const std::string& name, const glm::vec2& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::setVec4(const std::string& name, const glm::vec4& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    pugi::xml_node Shader::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName());
        node.append_attribute("assetId") = getId().c_str();
        node.append_attribute("name") = mName.c_str();
        node.append_attribute("srcVert") = mPath.string().c_str();
        node.append_attribute("srcFrag") = mFragmentPath.string().c_str();
        return node;
    }

}
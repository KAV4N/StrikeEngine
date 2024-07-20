#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {
    class Shader {
    public:
        Shader(const std::string& filepath);
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        void Bind();
        void Unbind();

        void GetAllUniformLocations();

        inline int GetUniformLocation(const std::string& uniformName) { return glGetUniformLocation(m_ProgramID, uniformName.c_str()); }
        inline void LoadFloat(int location, float value) { glUniform1f(location, value); }
        inline void LoadVector(int location, glm::vec3 vector) { glUniform3f(location, vector.x, vector.y, vector.z); }
        inline void LoadBoolean(int location, bool value) { glUniform1f(location, static_cast<float>(value)); }
        inline void LoadMatrix(int location, glm::mat4 matrix) { glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)); }

        inline void LoadTransformationMatrix(glm::mat4 matrix) { LoadMatrix(m_TransformationMatrix, matrix); }
        inline void LoadProjectionMatrix(glm::mat4 projection) { LoadMatrix(m_ProjectionMatrix, projection); }
        inline void LoadViewMatrix(glm::mat4 view) { LoadMatrix(m_ViewMatrix, view); }
        
        void LoadLight(int index, const glm::vec3& position, const glm::vec3& color, float intensity) {
            std::string prefix = "lights[" + std::to_string(index) + "]";
            LoadVector(GetUniformLocation(prefix + ".position"), position);
            LoadVector(GetUniformLocation(prefix + ".color"), color);
            LoadFloat(GetUniformLocation(prefix + ".intensity"), intensity);
        }

        inline void LoadViewPosition(glm::vec3 viewPosition) { LoadVector(m_ViewPosition, viewPosition); }

    private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        GLenum ShaderTypeFromString(const std::string& type);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    private:
        GLuint m_ProgramID;
        int m_TransformationMatrix;
        int m_ProjectionMatrix;
        int m_ViewMatrix;
        int m_ViewPosition;
    };
}

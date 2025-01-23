#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {
    class Texture;
    class Shader {
    public:
        Shader(GLuint programId, const std::string& name, const std::string& filepath);
        ~Shader();

        void Bind();
        void Unbind();

        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetBool(const std::string& name, bool value);

        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat2(const std::string& name, const glm::mat2& value);
        void SetMat3(const std::string& name, const glm::mat3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

        void SetIVec2(const std::string& name, const glm::ivec2& value);
        void SetIVec3(const std::string& name, const glm::ivec3& value);
        void SetIVec4(const std::string& name, const glm::ivec4& value);

        void SetBVec2(const std::string& name, const glm::bvec2& value);
        void SetBVec3(const std::string& name, const glm::bvec3& value);
        void SetBVec4(const std::string& name, const glm::bvec4& value);

        void SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values);
        void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& values);

        void SetTexture(const std::string& name, GLuint textureId);
        void SetStorageBuffer(const std::string& name, GLuint bufferId, GLuint bindingPoint);

        inline GLuint GetProgramId() const { return m_ProgramID; }
        inline std::string GetName() const { return m_Name; }
        inline std::string GetFilePath() const { return m_FilePath; }

    private:
        std::string m_Name;
        std::string m_FilePath;
        GLuint m_ProgramID;
    };
}

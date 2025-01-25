#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

namespace StrikeEngine {

    class Material {
    public:
        explicit Material(std::shared_ptr<Shader> shader);

        void Apply();

        template<typename T>
        void SetUniform(const std::string& name, const T& value);

        void AddTexture(const std::string& samplerName, std::shared_ptr<Texture> texture);
        void AddStorageBuffer(const std::string& name, GLuint bufferId, GLuint bindingPoint);

        std::shared_ptr<Shader> GetShader() const;

    private:
        struct StorageBufferBinding {
            GLuint BufferId;
            GLuint BindingPoint;
        };

        using UniformValue = std::variant<
            int,
            float,
            bool,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::ivec2,
            glm::ivec3,
            glm::ivec4,
            glm::bvec2,
            glm::bvec3,
            glm::bvec4,
            glm::mat2,
            glm::mat3,
            glm::mat4,
            std::vector<glm::vec3>,
            std::vector<glm::mat4>
        >;

        std::shared_ptr<Shader> m_Shader;
        std::unordered_map<std::string, UniformValue> m_Uniforms;
        std::vector<std::pair<std::string, std::shared_ptr<Texture>>> m_Textures;
        std::unordered_map<std::string, StorageBufferBinding> m_StorageBuffers;

        struct UniformVisitor {
            Shader* ShaderPtr;
            const std::string& Name;

            void operator()(int value) const;
            void operator()(float value) const;
            void operator()(bool value) const;
            void operator()(const glm::vec2& value) const;
            void operator()(const glm::vec3& value) const;
            void operator()(const glm::vec4& value) const;
            void operator()(const glm::ivec2& value) const;
            void operator()(const glm::ivec3& value) const;
            void operator()(const glm::ivec4& value) const;
            void operator()(const glm::bvec2& value) const;
            void operator()(const glm::bvec3& value) const;
            void operator()(const glm::bvec4& value) const;
            void operator()(const glm::mat2& value) const;
            void operator()(const glm::mat3& value) const;
            void operator()(const glm::mat4& value) const;
            void operator()(const std::vector<glm::vec3>& values) const;
            void operator()(const std::vector<glm::mat4>& values) const;
        };
    };

    template<typename T>
    void Material::SetUniform(const std::string& name, const T& value) {
        m_Uniforms[name] = value;
    }

}
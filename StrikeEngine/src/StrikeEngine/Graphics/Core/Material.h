#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <variant>

namespace StrikeEngine {
    class Shader;
    class Texture;

    class Material {
    public:
        enum class UniformType {
            Float, Vec2, Vec3, Vec4,
            Mat2, Mat3, Mat4,
            Int, IVec2, IVec3, IVec4,
            Bool, BVec2, BVec3, BVec4
        };

        explicit Material(std::shared_ptr<Shader> shader);

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&&) noexcept = default;
        Material& operator=(Material&&) noexcept = default;

        void Bind();
        void Unbind();

        template<typename T>
        void Set(const std::string& name, const T& value) {
            UniformType type = GetUniformType<T>();

            auto it = m_UniqueUniforms.find(name);

            if (it != m_UniqueUniforms.end()) {
                if (it->second.type != type) {
                    throw std::runtime_error("Attempting to set uniform '" + name +
                        "' with a different type than previously defined.");
                }
                it->second.value = value;
            }
            else {
                m_UniqueUniforms[name] = { name, type, value };
            }
        }

        void SetTexture(const std::string& name, std::shared_ptr<Texture> texture);
        void SetStorageBuffer(const std::string& name, GLuint bufferId);

    private:
        struct UniformInfo {
            std::string name;
            UniformType type;
            std::variant<
                float, glm::vec2, glm::vec3, glm::vec4,
                glm::mat2, glm::mat3, glm::mat4,
                int, glm::ivec2, glm::ivec3, glm::ivec4,
                bool, glm::bvec2, glm::bvec3, glm::bvec4
            > value;
        };

        struct TextureValue {
            std::shared_ptr<Texture> texture;
            unsigned int textureUnit;
        };

        struct StorageBufferValue {
            GLuint bufferId;
            GLuint bindingPoint;
        };

        template<typename T>
        UniformType GetUniformType() {
            if constexpr (std::is_same_v<T, float>)         return UniformType::Float;
            if constexpr (std::is_same_v<T, glm::vec2>)     return UniformType::Vec2;
            if constexpr (std::is_same_v<T, glm::vec3>)     return UniformType::Vec3;
            if constexpr (std::is_same_v<T, glm::vec4>)     return UniformType::Vec4;
            if constexpr (std::is_same_v<T, glm::mat2>)     return UniformType::Mat2;
            if constexpr (std::is_same_v<T, glm::mat3>)     return UniformType::Mat3;
            if constexpr (std::is_same_v<T, glm::mat4>)     return UniformType::Mat4;
            if constexpr (std::is_same_v<T, int>)           return UniformType::Int;
            if constexpr (std::is_same_v<T, glm::ivec2>)    return UniformType::IVec2;
            if constexpr (std::is_same_v<T, glm::ivec3>)    return UniformType::IVec3;
            if constexpr (std::is_same_v<T, glm::ivec4>)    return UniformType::IVec4;
            if constexpr (std::is_same_v<T, bool>)          return UniformType::Bool;
            if constexpr (std::is_same_v<T, glm::bvec2>)    return UniformType::BVec2;
            if constexpr (std::is_same_v<T, glm::bvec3>)    return UniformType::BVec3;
            if constexpr (std::is_same_v<T, glm::bvec4>)    return UniformType::BVec4;

            throw std::runtime_error("Unsupported uniform type");
        }

        void UploadUniforms();
        void BindTextures();
        void BindStorageBuffers();

        std::shared_ptr<Shader> m_Shader;

        std::unordered_map<std::string, UniformInfo> m_UniqueUniforms;

        std::unordered_map<std::string, TextureValue> m_Textures;
        std::unordered_map<std::string, StorageBufferValue> m_StorageBuffers;

        unsigned int m_NextTextureUnit;
        unsigned int m_NextStorageBuffer;
    };
}
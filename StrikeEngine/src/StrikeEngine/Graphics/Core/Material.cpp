#include "strikepch.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

namespace StrikeEngine {
    Material::Material(std::shared_ptr<Shader> shader)
        : m_Shader(shader), m_NextTextureUnit(0), m_NextStorageBuffer(0) {
    }

    void Material::Bind() {
        m_Shader->Bind();
        UploadUniforms();
        BindTextures();
        BindStorageBuffers();
    }

    void Material::Unbind() {
        m_Shader->Unbind();
    }

    void Material::SetTexture(const std::string& name, std::shared_ptr<Texture> texture) {
        TextureValue textureValue;
        textureValue.texture = std::move(texture);
        textureValue.textureUnit = m_NextTextureUnit++;
        m_Textures[name] = textureValue;
    }

    void Material::SetStorageBuffer(const std::string& name, GLuint bufferId) {
        StorageBufferValue buffer;
        buffer.bufferId = bufferId;
        buffer.bindingPoint = m_NextStorageBuffer++;
        m_StorageBuffers[name] = buffer;
    }

    void Material::UploadUniforms() {
        const std::unordered_map<UniformType, std::function<void(const std::string&, const std::variant<float, glm::vec2, glm::vec3, glm::vec4,
            glm::mat2, glm::mat3, glm::mat4,
            int, glm::ivec2, glm::ivec3, glm::ivec4,
            bool, glm::bvec2, glm::bvec3, glm::bvec4>&)>> uniformSetters = {
                {UniformType::Float,   [this](const std::string& name, const auto& value) { m_Shader->SetFloat(name, std::get<float>(value)); }},
                {UniformType::Vec2,    [this](const std::string& name, const auto& value) { m_Shader->SetVec2(name, std::get<glm::vec2>(value)); }},
                {UniformType::Vec3,    [this](const std::string& name, const auto& value) { m_Shader->SetVec3(name, std::get<glm::vec3>(value)); }},
                {UniformType::Vec4,    [this](const std::string& name, const auto& value) { m_Shader->SetVec4(name, std::get<glm::vec4>(value)); }},
                {UniformType::Mat2,    [this](const std::string& name, const auto& value) { m_Shader->SetMat2(name, std::get<glm::mat2>(value)); }},
                {UniformType::Mat3,    [this](const std::string& name, const auto& value) { m_Shader->SetMat3(name, std::get<glm::mat3>(value)); }},
                {UniformType::Mat4,    [this](const std::string& name, const auto& value) { m_Shader->SetMat4(name, std::get<glm::mat4>(value)); }},
                {UniformType::Int,     [this](const std::string& name, const auto& value) { m_Shader->SetInt(name, std::get<int>(value)); }},
                {UniformType::IVec2,   [this](const std::string& name, const auto& value) { m_Shader->SetIVec2(name, std::get<glm::ivec2>(value)); }},
                {UniformType::IVec3,   [this](const std::string& name, const auto& value) { m_Shader->SetIVec3(name, std::get<glm::ivec3>(value)); }},
                {UniformType::IVec4,   [this](const std::string& name, const auto& value) { m_Shader->SetIVec4(name, std::get<glm::ivec4>(value)); }},
                {UniformType::Bool,    [this](const std::string& name, const auto& value) { m_Shader->SetBool(name, std::get<bool>(value)); }},
                {UniformType::BVec2,   [this](const std::string& name, const auto& value) { m_Shader->SetBVec2(name, std::get<glm::bvec2>(value)); }},
                {UniformType::BVec3,   [this](const std::string& name, const auto& value) { m_Shader->SetBVec3(name, std::get<glm::bvec3>(value)); }},
                {UniformType::BVec4,   [this](const std::string& name, const auto& value) { m_Shader->SetBVec4(name, std::get<glm::bvec4>(value)); }}
        };

        // Iterate through unique uniforms instead of m_UniformStorage
        for (const auto& [name, uniform] : m_UniqueUniforms) {
            uniformSetters.at(uniform.type)(uniform.name, uniform.value);
        }
    }

    void Material::BindTextures() {
        for (const auto& [name, textureValue] : m_Textures) {
            textureValue.texture->Bind(textureValue.textureUnit);
            m_Shader->SetTexture(name, textureValue.textureUnit);
        }
    }

    void Material::BindStorageBuffers() {
        for (const auto& [name, buffer] : m_StorageBuffers) {
            m_Shader->SetStorageBuffer(name, buffer.bufferId, buffer.bindingPoint);
        }
    }
}
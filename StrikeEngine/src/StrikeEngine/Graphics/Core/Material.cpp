#include "strikepch.h"
#include "Material.h"
#include <glad/glad.h>

namespace StrikeEngine {

    Material::Material(std::shared_ptr<Shader> shader)
        : m_Shader(std::move(shader)) {
    }

    void Material::Apply() {
        if (!m_Shader) return;

        m_Shader->Bind();

        for (const auto& [name, value] : m_Uniforms) {
            std::visit(UniformVisitor{ m_Shader.get(), name }, value);
        }


        for (const auto& [samplerName, texture] : m_Textures) {
            if (texture) {
                texture->Bind();
                GLuint unitIndex = texture->GetUnit();
                m_Shader->SetInt(samplerName, unitIndex);
            }
        }

        for (const auto& [name, binding] : m_StorageBuffers) {
            m_Shader->SetStorageBuffer(name, binding.BufferId, binding.BindingPoint);
        }
    }

    void Material::AddTexture(const std::string& samplerName, std::shared_ptr<Texture> texture) {
        m_Textures.emplace_back(samplerName, std::move(texture));
    }

    void Material::AddStorageBuffer(const std::string& name, GLuint bufferId, GLuint bindingPoint) {
        m_StorageBuffers[name] = { bufferId, bindingPoint };
    }

    std::shared_ptr<Shader> Material::GetShader() const {
        return m_Shader;
    }

    void Material::UniformVisitor::operator()(int value) const {
        ShaderPtr->SetInt(Name, value);
    }

    void Material::UniformVisitor::operator()(float value) const {
        ShaderPtr->SetFloat(Name, value);
    }

    void Material::UniformVisitor::operator()(bool value) const {
        ShaderPtr->SetBool(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::vec2& value) const {
        ShaderPtr->SetVec2(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::vec3& value) const {
        ShaderPtr->SetVec3(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::vec4& value) const {
        ShaderPtr->SetVec4(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::ivec2& value) const {
        ShaderPtr->SetIVec2(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::ivec3& value) const {
        ShaderPtr->SetIVec3(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::ivec4& value) const {
        ShaderPtr->SetIVec4(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::bvec2& value) const {
        ShaderPtr->SetBVec2(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::bvec3& value) const {
        ShaderPtr->SetBVec3(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::bvec4& value) const {
        ShaderPtr->SetBVec4(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::mat2& value) const {
        ShaderPtr->SetMat2(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::mat3& value) const {
        ShaderPtr->SetMat3(Name, value);
    }

    void Material::UniformVisitor::operator()(const glm::mat4& value) const {
        ShaderPtr->SetMat4(Name, value);
    }

    void Material::UniformVisitor::operator()(const std::vector<glm::vec3>& values) const {
        ShaderPtr->SetVec3Array(Name, values);
    }

    void Material::UniformVisitor::operator()(const std::vector<glm::mat4>& values) const {
        ShaderPtr->SetMat4Array(Name, values);
    }

}
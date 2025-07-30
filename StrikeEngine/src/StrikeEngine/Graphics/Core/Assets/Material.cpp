#include "Material.h"

namespace StrikeEngine {

    Material::Material(const std::string& id, const std::string& name)
        : Asset(id), m_name(name), m_diffuseColor(1.0f), m_specularColor(1.0f),
        m_ambientColor(1.0f), m_shininess(32.0f) {
    }

    // Getters
    const std::string& Material::GetName() const { return m_name; }
    const glm::vec3& Material::GetDiffuseColor() const { return m_diffuseColor; }
    const glm::vec3& Material::GetSpecularColor() const { return m_specularColor; }
    const glm::vec3& Material::GetAmbientColor() const { return m_ambientColor; }
    float Material::GetShininess() const { return m_shininess; }
    const std::string& Material::GetDiffuseTexture() const { return m_diffuseTexture; }
    const std::string& Material::GetSpecularTexture() const { return m_specularTexture; }
    const std::string& Material::GetNormalTexture() const { return m_normalTexture; }

    // Setters
    void Material::SetName(const std::string& name) { m_name = name; }
    void Material::SetDiffuseColor(const glm::vec3& color) { m_diffuseColor = color; }
    void Material::SetSpecularColor(const glm::vec3& color) { m_specularColor = color; }
    void Material::SetAmbientColor(const glm::vec3& color) { m_ambientColor = color; }
    void Material::SetShininess(float shininess) { m_shininess = shininess; }
    void Material::SetDiffuseTexture(const std::string& texture) { m_diffuseTexture = texture; }
    void Material::SetSpecularTexture(const std::string& texture) { m_specularTexture = texture; }
    void Material::SetNormalTexture(const std::string& texture) { m_normalTexture = texture; }

}

#pragma once
#include "StrikeEngine/Core/UUID.h"
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Asset.h"

namespace StrikeEngine {
    class Material : public Asset {
    public:
        Material(const std::string& id, const std::string& name);

        static AssetType GetStaticAssetType() { return AssetType::MATERIAL; }

        // Getters
        const std::string& GetName() const;
        const glm::vec3& GetDiffuseColor() const;
        const glm::vec3& GetSpecularColor() const;
        const glm::vec3& GetAmbientColor() const;
        float GetShininess() const;
        const std::string& GetDiffuseTexture() const;
        const std::string& GetSpecularTexture() const;
        const std::string& GetNormalTexture() const;

        // Setters
        void SetName(const std::string& name);
        void SetDiffuseColor(const glm::vec3& color);
        void SetSpecularColor(const glm::vec3& color);
        void SetAmbientColor(const glm::vec3& color);
        void SetShininess(float shininess);
        void SetDiffuseTexture(const std::string& texture);
        void SetSpecularTexture(const std::string& texture);
        void SetNormalTexture(const std::string& texture);

    private:
        std::string m_name;
        glm::vec3 m_diffuseColor;
        glm::vec3 m_specularColor;
        glm::vec3 m_ambientColor;
        float m_shininess;
        std::string m_diffuseTexture;
        std::string m_specularTexture;
        std::string m_normalTexture;
    };
}

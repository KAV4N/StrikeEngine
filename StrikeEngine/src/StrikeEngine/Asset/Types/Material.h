#pragma once
#include "StrikeEngine/Core/UUID.h"
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Asset.h"

namespace StrikeEngine {
    class Material : public Asset {
    public:
        Material(const std::string& id, const std::filesystem::path& path, const std::string& name = "");

        static AssetType getStaticAssetType() { return AssetType::Material; }

        bool swapData(Asset& other) override;

        const glm::vec3& getDiffuseColor() const;
        const glm::vec3& getSpecularColor() const;
        const glm::vec3& getAmbientColor() const;
        float getShininess() const;
        const std::string& getDiffuseTexture() const;
        const std::string& getSpecularTexture() const;
        const std::string& getNormalTexture() const;

        void setDiffuseColor(const glm::vec3& color);
        void setSpecularColor(const glm::vec3& color);
        void setAmbientColor(const glm::vec3& color);
        void setShininess(float shininess);
        void setDiffuseTexture(const std::string& texture);
        void setSpecularTexture(const std::string& texture);
        void setNormalTexture(const std::string& texture);

    private:
        std::string mName;
        glm::vec3 mDiffuseColor;
        glm::vec3 mSpecularColor;
        glm::vec3 mAmbientColor;
        float mShininess;
        std::string mDiffuseTexture;
        std::string mSpecularTexture;
        std::string mNormalTexture;
    };
}
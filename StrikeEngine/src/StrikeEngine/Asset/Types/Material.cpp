#include "Material.h"

namespace StrikeEngine {

    Material::Material(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name), mDiffuseColor(1.0f), mSpecularColor(1.0f),
        mAmbientColor(1.0f), mShininess(32.0f) {
    }

    bool Material::swapData(Asset& other) {
        if (other.getTypeName() != getTypeName()) {
            return false;
        }

        Material* otherMaterial = dynamic_cast<Material*>(&other);
        if (!otherMaterial) {
            return false;
        }
        std::swap(mName, otherMaterial->mName);
        std::swap(mDiffuseColor, otherMaterial->mDiffuseColor);
        std::swap(mSpecularColor, otherMaterial->mSpecularColor);
        std::swap(mAmbientColor, otherMaterial->mAmbientColor);
        std::swap(mShininess, otherMaterial->mShininess);
        std::swap(mDiffuseTexture, otherMaterial->mDiffuseTexture);
        std::swap(mSpecularTexture, otherMaterial->mSpecularTexture);
        std::swap(mNormalTexture, otherMaterial->mNormalTexture);

        return true;
    }

    const glm::vec3& Material::getDiffuseColor() const { return mDiffuseColor; }
    const glm::vec3& Material::getSpecularColor() const { return mSpecularColor; }
    const glm::vec3& Material::getAmbientColor() const { return mAmbientColor; }
    float Material::getShininess() const { return mShininess; }
    const std::string& Material::getDiffuseTexture() const { return mDiffuseTexture; }
    const std::string& Material::getSpecularTexture() const { return mSpecularTexture; }
    const std::string& Material::getNormalTexture() const { return mNormalTexture; }

    void Material::setDiffuseColor(const glm::vec3& color) { mDiffuseColor = color; }
    void Material::setSpecularColor(const glm::vec3& color) { mSpecularColor = color; }
    void Material::setAmbientColor(const glm::vec3& color) { mAmbientColor = color; }
    void Material::setShininess(float shininess) { mShininess = shininess; }
    void Material::setDiffuseTexture(const std::string& texture) { mDiffuseTexture = texture; }
    void Material::setSpecularTexture(const std::string& texture) { mSpecularTexture = texture; }
    void Material::setNormalTexture(const std::string& texture) { mNormalTexture = texture; }
}
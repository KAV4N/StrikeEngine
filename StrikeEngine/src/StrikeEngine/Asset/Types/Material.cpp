#include "strikepch.h"
#include "Material.h"
#include "StrikeEngine/Graphics/Shader.h"
#include <pugixml.hpp>

namespace StrikeEngine {

    Material::Material(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path),
        mShader(nullptr),
        mBaseColor(255, 255, 255),  // White
        mMetallic(0.0f),             // Non-metallic
        mRoughness(0.5f)             // Medium roughness
    {
        mShader = ShaderManager::get().getShader("defaultPBR.glsl");
    }

    const std::string& Material::getStaticTypeName() {
        static const std::string typeName = "material";
        return typeName;
    }

    const std::string& Material::getTypeName() const {
        return getStaticTypeName();
    }

    // PBR Property setters
    void Material::setBaseColor(const glm::vec3& baseColor) {
        mBaseColor = glm::clamp(baseColor, glm::vec3(0), glm::vec3(255));
    }

    void Material::setMetallic(float metallic) {
        mMetallic = glm::clamp(metallic, 0.0f, 1.0f);
    }

    void Material::setRoughness(float roughness) {
        mRoughness = glm::clamp(roughness, 0.0f, 1.0f);
    }

    // PBR Property getters
    glm::vec3 Material::getBaseColor() const {
        return mBaseColor;
    }

    float Material::getMetallic() const {
        return mMetallic;
    }

    float Material::getRoughness() const {
        return mRoughness;
    }

    // Texture setters
    void Material::setBaseColorTexture(std::shared_ptr<Texture> texture) {
        setTexture(TextureSlot::BaseColor, texture);
    }

    void Material::setNormalTexture(std::shared_ptr<Texture> texture) {
        setTexture(TextureSlot::Normal, texture);
    }

    void Material::setMetallicTexture(std::shared_ptr<Texture> texture) {
        setTexture(TextureSlot::Metallic, texture);
    }

    void Material::setRoughnessTexture(std::shared_ptr<Texture> texture) {
        setTexture(TextureSlot::Roughness, texture);
    }

    // Texture getters
    std::shared_ptr<Texture> Material::getBaseColorTexture() const {
        return getTexture(TextureSlot::BaseColor);
    }

    std::shared_ptr<Texture> Material::getNormalTexture() const {
        return getTexture(TextureSlot::Normal);
    }

    std::shared_ptr<Texture> Material::getMetallicTexture() const {
        return getTexture(TextureSlot::Metallic);
    }

    std::shared_ptr<Texture> Material::getRoughnessTexture() const {
        return getTexture(TextureSlot::Roughness);
    }

    // Texture existence checks
    bool Material::hasBaseColorTexture() const {
        return hasTexture(TextureSlot::BaseColor);
    }

    bool Material::hasNormalTexture() const {
        return hasTexture(TextureSlot::Normal);
    }

    bool Material::hasMetallicTexture() const {
        return hasTexture(TextureSlot::Metallic);
    }

    bool Material::hasRoughnessTexture() const {
        return hasTexture(TextureSlot::Roughness);
    }

    // Shader management
    std::shared_ptr<Shader> Material::getShader() const {
        return mShader;
    }

    // Private texture management
    void Material::setTexture(TextureSlot slot, std::shared_ptr<Texture> texture) {
        uint32_t slotIndex = static_cast<uint32_t>(slot);
        if (texture) {
            mTextures[slotIndex] = texture;
        } else {
            mTextures.erase(slotIndex);
        }
    }

    std::shared_ptr<Texture> Material::getTexture(TextureSlot slot) const {
        uint32_t slotIndex = static_cast<uint32_t>(slot);
        auto it = mTextures.find(slotIndex);
        return it != mTextures.end() ? it->second : nullptr;
    }

    bool Material::hasTexture(TextureSlot slot) const {
        uint32_t slotIndex = static_cast<uint32_t>(slot);
        return mTextures.find(slotIndex) != mTextures.end();
    }

    void Material::bind() const {
        if (!mShader) {
            throw std::runtime_error("Material '" + mId + "' has no shader assigned or is loading");
        }
        
        mShader->bind();
        
        // Bind textures to their predefined slots
        for (const auto& [slot, texture] : mTextures) {
            if (texture) {
                texture->bind(slot);
            }
        }

        // Set PBR material properties (convert baseColor to 0-1 range for shader)
        mShader->setVec3("uMaterial.baseColor", glm::vec3(mBaseColor) / 255.0f);
        mShader->setFloat("uMaterial.metallic", mMetallic);
        mShader->setFloat("uMaterial.roughness", mRoughness);

        // Set texture presence flags
        mShader->setInt("uMaterial.hasBaseColorMap", hasBaseColorTexture() ? 1 : 0);
        mShader->setInt("uMaterial.hasNormalMap", hasNormalTexture() ? 1 : 0);
        mShader->setInt("uMaterial.hasMetallicMap", hasMetallicTexture() ? 1 : 0);
        mShader->setInt("uMaterial.hasRoughnessMap", hasRoughnessTexture() ? 1 : 0);

        // Set texture samplers
        mShader->setInt("uMaterial.baseColorMap", static_cast<int>(TextureSlot::BaseColor));
        mShader->setInt("uMaterial.normalMap", static_cast<int>(TextureSlot::Normal));
        mShader->setInt("uMaterial.metallicMap", static_cast<int>(TextureSlot::Metallic));
        mShader->setInt("uMaterial.roughnessMap", static_cast<int>(TextureSlot::Roughness));
        
    }

    void Material::unbind() const {
        /*
        for (const auto& [slot, texture] : mTextures) {
            if (texture) {
                texture->unbind();
            }
        }
        */
        if (mShader) {
            mShader->unbind();
        }
        
    }

    void Material::serialize(const std::filesystem::path& path) const {
        pugi::xml_document doc;
        
        pugi::xml_node declaration = doc.prepend_child(pugi::node_declaration);
        declaration.append_attribute("version") = "1.0";

        pugi::xml_node materialNode = doc.append_child("material");

        pugi::xml_node baseColorNode = materialNode.append_child("baseColor");
        baseColorNode.append_attribute("r") = mBaseColor.r;
        baseColorNode.append_attribute("g") = mBaseColor.g;
        baseColorNode.append_attribute("b") = mBaseColor.b;
        
        if (hasBaseColorTexture()) {
            pugi::xml_node textureData = getBaseColorTexture()->toNode();
            baseColorNode.append_copy(textureData);
        }

        pugi::xml_node normalNode = materialNode.append_child("normal");
        if (hasNormalTexture()) {
            pugi::xml_node textureData = getNormalTexture()->toNode();
            normalNode.append_copy(textureData);
        }

        pugi::xml_node metallicNode = materialNode.append_child("metallic");
        metallicNode.append_attribute("value") = mMetallic;
        if (hasMetallicTexture()) {
            pugi::xml_node textureData = getMetallicTexture()->toNode();
            metallicNode.append_copy(textureData);
        }

        pugi::xml_node roughnessNode = materialNode.append_child("roughness");
        roughnessNode.append_attribute("value") = mRoughness;
        if (hasRoughnessTexture()) {
            pugi::xml_node textureData = getRoughnessTexture()->toNode();
            roughnessNode.append_copy(textureData);
        }

        doc.save_file(path.c_str());
    }
}
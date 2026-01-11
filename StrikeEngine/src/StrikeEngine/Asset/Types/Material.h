#pragma once

#include "Asset.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace StrikeEngine {

    class Shader;

    enum class TextureSlot : uint32_t {
        BaseColor = 1,
        Metallic = 3,   
        Roughness = 4,
    };

    class Material : public Asset {
    public:
        Material(const std::string& id, const std::filesystem::path& path);

        static const std::string& getStaticTypeName();
        const std::string& getTypeName() const override;

        // PBR Property setters
        void setBaseColor(const glm::vec3& baseColor);
        void setMetallic(float metallic);
        void setRoughness(float roughness);

        // PBR Property getters
        glm::vec3 getBaseColor() const;
        float getMetallic() const;
        float getRoughness() const;

        // Texture ID setters - return true if texture exists and was assigned
        bool setBaseColorTextureById(const std::string& textureId);
        bool setMetallicTextureById(const std::string& textureId);
        bool setRoughnessTextureById(const std::string& textureId);

        // Texture getters
        const std::shared_ptr<Texture> getBaseColorTexture() const;
        const std::shared_ptr<Texture> getMetallicTexture() const;
        const std::shared_ptr<Texture> getRoughnessTexture() const;

        // Texture existence checks
        bool hasBaseColorTexture() const;
        bool hasMetallicTexture() const;
        bool hasRoughnessTexture() const;

        // Shader management
        std::shared_ptr<Shader> getShader() const;

        void bind() const;
        void unbind() const;

        void serialize(const std::filesystem::path& path) const;

    private:
        friend class ModelParser;
        bool setTextureById(TextureSlot slot, const std::string& textureId);
        void setTexture(TextureSlot slot, std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> getTexture(TextureSlot slot) const;
        bool hasTexture(TextureSlot slot) const;

        std::shared_ptr<Shader> mShader;
        std::unordered_map<uint32_t, std::shared_ptr<Texture>> mTextures;
        
        glm::vec3 mBaseColor;
        float mMetallic;
        float mRoughness;
    };

}
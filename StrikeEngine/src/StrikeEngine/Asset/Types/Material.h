#pragma once
#include "Asset.h"
#include "Texture.h"
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>

namespace StrikeEngine {
    class Shader;

    class Material : public Asset {
    public:
        // PBR Texture slots (predefined to match shader)
        enum class TextureSlot : uint32_t {
            BaseColor = 2,
            Normal = 3,
            Metallic = 4,
            Roughness = 5
        };

        Material(const std::string& id, const std::filesystem::path& path);

        static const std::string& getStaticTypeName();
        const std::string& getTypeName() const override;

        // PBR Property setters
        void setBaseColor(const glm::vec3& baseColor);  // Changed to vec3 [0,1] range
        void setMetallic(float metallic);
        void setRoughness(float roughness);

        // PBR Property getters
        glm::vec3 getBaseColor() const;
        float getMetallic() const;
        float getRoughness() const;

        // Texture management
        void setBaseColorTexture(std::shared_ptr<Texture> texture);
        void setNormalTexture(std::shared_ptr<Texture> texture);
        void setMetallicTexture(std::shared_ptr<Texture> texture);
        void setRoughnessTexture(std::shared_ptr<Texture> texture);

        std::shared_ptr<Texture> getBaseColorTexture() const;
        std::shared_ptr<Texture> getNormalTexture() const;
        std::shared_ptr<Texture> getMetallicTexture() const;
        std::shared_ptr<Texture> getRoughnessTexture() const;

        // Check if textures are present
        bool hasBaseColorTexture() const;
        bool hasNormalTexture() const;
        bool hasMetallicTexture() const;
        bool hasRoughnessTexture() const;

        // Shader management
        std::shared_ptr<Shader> getShader() const;

        // Utility methods
        void serialize(const std::filesystem::path& path) const;
        void bind() const;
        void unbind() const;

    private:
        void setTexture(TextureSlot slot, std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> getTexture(TextureSlot slot) const;
        bool hasTexture(TextureSlot slot) const;

    private:
        std::shared_ptr<Shader> mShader;
        std::unordered_map<uint32_t, std::shared_ptr<Texture>> mTextures;

        glm::vec3 mBaseColor;  
        float mMetallic;
        float mRoughness;
    };
}
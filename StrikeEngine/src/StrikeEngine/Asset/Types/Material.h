#pragma once
#include "StrikeEngine/Core/UUID.h"
#include <string>
#include <memory>
#include <variant>
#include <unordered_map>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include "Asset.h"
#include "Shader.h"
#include "Texture2D.h"

namespace StrikeEngine {
    class Material : public Asset {
    public:
        using UniformValue = std::variant<
            int,
            float,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::mat4,
            std::vector<int>
        >;

        Material(const std::string& id, const std::filesystem::path& path, const std::string& name = "");

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "material";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        // Uniform setters
        void setInt(const std::string& name, int value);
        void setIntArray(const std::string& name, int* values, uint32_t count);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setMat4(const std::string& name, const glm::mat4& value);

        // Individual uniform getters
        std::optional<int> getInt(const std::string& name) const;
        std::optional<std::vector<int>> getIntArray(const std::string& name) const;
        std::optional<float> getFloat(const std::string& name) const;
        std::optional<glm::vec2> getVec2(const std::string& name) const;
        std::optional<glm::vec3> getVec3(const std::string& name) const;
        std::optional<glm::vec4> getVec4(const std::string& name) const;
        std::optional<glm::mat4> getMat4(const std::string& name) const;

        // Get all uniforms
        const std::unordered_map<std::string, UniformValue>& getUniforms() const { return mUniforms; }

        // Get uniform by name (returns the variant directly)
        std::optional<UniformValue> getUniform(const std::string& name) const;

        // Check if uniform exists
        bool hasUniform(const std::string& name) const { return mUniforms.find(name) != mUniforms.end(); }

        // Get uniform names
        std::vector<std::string> getUniformNames() const;

        // Texture management
        void addTexture(uint32_t slot, std::shared_ptr<Texture2D> texture);
        void setTextures(const std::vector<std::pair<uint32_t, std::shared_ptr<Texture2D>>>& textures);
        void removeTexture(uint32_t slot);
        void setTextureSlot(uint32_t oldSlot, uint32_t newSlot);
        const std::unordered_map<uint32_t, std::shared_ptr<Texture2D>>& getTextures() const { return mTextures; }
        std::shared_ptr<Texture2D> getTexture(uint32_t slot) const;

        // Shader management
        void setShader(std::shared_ptr<Shader> shader);
        std::shared_ptr<Shader> getShader() const { return mShader; }

        // Utility methods
        void serialize(const std::filesystem::path& path) const;
        void bind() const;
        void unbind() const;
        void clearUniforms();
        void clearTextures();

    private:
        void applyUniform(const std::string& name, const UniformValue& value) const;

    private:
        std::shared_ptr<Shader> mShader;
        std::unordered_map<uint32_t, std::shared_ptr<Texture2D>> mTextures;
        std::unordered_map<std::string, UniformValue> mUniforms;
    };
}
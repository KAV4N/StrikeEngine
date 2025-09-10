#include "Material.h"
#include <iostream>

namespace StrikeEngine {

    Material::Material(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name)
    {
    }

    void Material::setInt(const std::string& name, int value) {
        mUniforms[name] = value;
    }

    void Material::setIntArray(const std::string& name, int* values, uint32_t count) {
        std::vector<int> intArray(values, values + count);
        mUniforms[name] = intArray;
    }

    void Material::setFloat(const std::string& name, float value) {
        mUniforms[name] = value;
    }

    void Material::setVec2(const std::string& name, const glm::vec2& value) {
        mUniforms[name] = value;
    }

    void Material::setVec3(const std::string& name, const glm::vec3& value) {
        mUniforms[name] = value;
    }

    void Material::setVec4(const std::string& name, const glm::vec4& value) {
        mUniforms[name] = value;
    }

    void Material::setMat4(const std::string& name, const glm::mat4& value) {
        mUniforms[name] = value;
    }

    void Material::addTexture(uint32_t slot, std::shared_ptr<Texture2D> texture) {
        mTextures[slot] = texture;
    }

    void Material::removeTexture(uint32_t slot) {
        auto it = mTextures.find(slot);
        if (it != mTextures.end()) {
            mTextures.erase(it);
        }
    }


    void Material::setShader(std::shared_ptr<Shader> shader) {
        mShader = shader;
    }

    void Material::bind() const {
        if (!mShader) {
            std::cerr << "Warning: Material '" << mName << "' has no shader assigned!" << std::endl;
            return;
        }

        mShader->bind();

        for (const auto& [slot, texture] : mTextures) {
            if (texture) {
                glActiveTexture(GL_TEXTURE0 + slot);
                texture->bind();
            }
        }

        for (const auto& [name, value] : mUniforms) {
            applyUniform(name, value);
        }
    }

    void Material::unbind() const {
        for (const auto& [slot, texture] : mTextures) {
            if (texture) {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        if (mShader) {
            mShader->unbind();
        }
    }

    void Material::applyUniform(const std::string& name, const UniformValue& value) const {
        std::visit([this, &name](const auto& val) {
            using T = std::decay_t<decltype(val)>;

            if constexpr (std::is_same_v<T, int>) {
                mShader->setInt(name, val);
            }
            else if constexpr (std::is_same_v<T, float>) {
                mShader->setFloat(name, val);
            }
            else if constexpr (std::is_same_v<T, glm::vec2>) {
                mShader->setVec2(name, val);
            }
            else if constexpr (std::is_same_v<T, glm::vec3>) {
                mShader->setVec3(name, val);
            }
            else if constexpr (std::is_same_v<T, glm::vec4>) {
                mShader->setVec4(name, val);
            }
            else if constexpr (std::is_same_v<T, glm::mat4>) {
                mShader->setMat4(name, val);
            }
            else if constexpr (std::is_same_v<T, std::vector<int>>) {
                mShader->setIntArray(name, const_cast<int*>(val.data()), static_cast<uint32_t>(val.size()));
            }
            }, value);
    }

    void Material::clearUniforms() {
        mUniforms.clear();
    }

    void Material::clearTextures() {
        mTextures.clear();
    }

}
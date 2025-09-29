#include "Material.h"
#include <iostream>
#include <optional>
#include <sstream>

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

    std::optional<int> Material::getInt(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<int>(it->second)) {
            return std::get<int>(it->second);
        }
        return std::nullopt;
    }

    std::optional<std::vector<int>> Material::getIntArray(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<std::vector<int>>(it->second)) {
            return std::get<std::vector<int>>(it->second);
        }
        return std::nullopt;
    }

    std::optional<float> Material::getFloat(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<float>(it->second)) {
            return std::get<float>(it->second);
        }
        return std::nullopt;
    }

    std::optional<glm::vec2> Material::getVec2(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<glm::vec2>(it->second)) {
            return std::get<glm::vec2>(it->second);
        }
        return std::nullopt;
    }

    std::optional<glm::vec3> Material::getVec3(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<glm::vec3>(it->second)) {
            return std::get<glm::vec3>(it->second);
        }
        return std::nullopt;
    }

    std::optional<glm::vec4> Material::getVec4(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<glm::vec4>(it->second)) {
            return std::get<glm::vec4>(it->second);
        }
        return std::nullopt;
    }

    std::optional<glm::mat4> Material::getMat4(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end() && std::holds_alternative<glm::mat4>(it->second)) {
            return std::get<glm::mat4>(it->second);
        }
        return std::nullopt;
    }

    void Material::addTexture(uint32_t slot, std::shared_ptr<Texture2D> texture) {
        mTextures[slot] = texture;
    }

    void Material::setTextures(const std::vector<std::pair<uint32_t, std::shared_ptr<Texture2D>>>& textures) {
        for (const auto& [slot, texture] : textures) {
            if (texture) {
                mTextures[slot] = texture;
            }
            else {
                mTextures.erase(slot);
            }
        }
    }

    void Material::removeTexture(uint32_t slot) {
        auto it = mTextures.find(slot);
        if (it != mTextures.end()) {
            mTextures.erase(it);
        }
    }

    void Material::setTextureSlot(uint32_t oldSlot, uint32_t newSlot) {
        auto it = mTextures.find(oldSlot);
        if (it != mTextures.end()) {
            auto texture = it->second;
            mTextures.erase(it);
            mTextures[newSlot] = texture;
        }
    }

    std::shared_ptr<Texture2D> Material::getTexture(uint32_t slot) const {
        auto it = mTextures.find(slot);
        return it != mTextures.end() ? it->second : nullptr;
    }

    void Material::setShader(std::shared_ptr<Shader> shader) {
        mShader = shader;
    }

    void Material::bind() const {
        if (!mShader || !mShader->isReady()) {
            std::cerr << "Warning: Material '" << mName << "' has no shader assigned or is loading" << std::endl;
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
            applyUniform("uMaterial." + name, value);
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

    void Material::serialize(const std::filesystem::path& path) const {
        pugi::xml_document doc;

        // Add XML declaration
        pugi::xml_node declaration = doc.prepend_child(pugi::node_declaration);
        declaration.append_attribute("version") = "1.0";

        pugi::xml_node materialNode = doc.append_child("material");

        // Serialize shader
        if (mShader) {
            pugi::xml_node shaderNode = materialNode.append_child("shader");
            shaderNode.append_attribute("assetId") = mShader->getId().c_str();
            shaderNode.append_attribute("srcVert") = mShader->getPath().string().c_str();
            shaderNode.append_attribute("srcFrag") = mShader->getFragmentPath().string().c_str();
        }

        // Serialize properties
        if (!mUniforms.empty()) {
            pugi::xml_node propertiesNode = materialNode.append_child("properties");

            for (const auto& [name, value] : mUniforms) {
                pugi::xml_node propertyNode = propertiesNode.append_child("property");
                propertyNode.append_attribute("name") = name.c_str();

                std::visit([&propertyNode](const auto& val) {
                    using T = std::decay_t<decltype(val)>;

                    if constexpr (std::is_same_v<T, int>) {
                        propertyNode.append_attribute("type") = "int";
                        propertyNode.append_attribute("value") = val;
                    }
                    else if constexpr (std::is_same_v<T, float>) {
                        propertyNode.append_attribute("type") = "float";
                        propertyNode.append_attribute("value") = val;
                    }
                    else if constexpr (std::is_same_v<T, glm::vec2>) {
                        propertyNode.append_attribute("type") = "vec2";
                        std::string vecStr = std::to_string(val.x) + "," + std::to_string(val.y);
                        propertyNode.append_attribute("value") = vecStr.c_str();
                    }
                    else if constexpr (std::is_same_v<T, glm::vec3>) {
                        propertyNode.append_attribute("type") = "vec3";
                        std::string vecStr = std::to_string(val.x) + "," + std::to_string(val.y) + "," + std::to_string(val.z);
                        propertyNode.append_attribute("value") = vecStr.c_str();
                    }
                    else if constexpr (std::is_same_v<T, glm::vec4>) {
                        propertyNode.append_attribute("type") = "vec4";
                        std::string vecStr = std::to_string(val.x) + "," + std::to_string(val.y) + "," + std::to_string(val.z) + "," + std::to_string(val.w);
                        propertyNode.append_attribute("value") = vecStr.c_str();
                    }
                }, value);
            }
        }

        // Serialize textures
        if (!mTextures.empty()) {
            pugi::xml_node texturesNode = materialNode.append_child("textures");

            for (const auto& [slot, texture] : mTextures) {
                if (texture) {
                    pugi::xml_node textureNode = texturesNode.append_child("texture2D");
                    textureNode.append_attribute("name") = ("texture_" + std::to_string(slot)).c_str();
                    textureNode.append_attribute("src") = texture->getPath().string().c_str();
                }
            }
        }

        doc.save_file(path.c_str());
    }

    std::optional<Material::UniformValue> Material::getUniform(const std::string& name) const {
        auto it = mUniforms.find(name);
        if (it != mUniforms.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<std::string> Material::getUniformNames() const {
        std::vector<std::string> names;
        names.reserve(mUniforms.size());

        for (const auto& [name, value] : mUniforms) {
            names.push_back(name);
        }

        return names;
    }
}
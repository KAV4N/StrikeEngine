#include "strikepch.h"
#include "RendererComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    REGISTER_COMPONENT(RendererComponent)

    RendererComponent::RendererComponent()
        : mColor(255, 255, 255) // Default white color
    {}

    void RendererComponent::setModel(const std::string& modelId) {
        mModelId = modelId;
        mMeshIdx.reset();
    }

    void RendererComponent::removeModel() {
        mModelId.clear();
        mMeshIdx.reset();
    }

    bool RendererComponent::hasModel() const {
        return !mModelId.empty();
    }

    std::shared_ptr<Model> RendererComponent::getModel() const {
        if (mModelId.empty()) {
            return nullptr;
        }
        return AssetManager::get().getAsset<Model>(mModelId);
    }

    void RendererComponent::setTexture(const std::string& textureId) {
        mTextureId = textureId;
    }

    void RendererComponent::removeTexture() {
        mTextureId.clear();
    }

    bool RendererComponent::hasTexture() const {
        return !mTextureId.empty();
    }

    std::shared_ptr<Texture> RendererComponent::getTexture() const {
        if (mTextureId.empty()) {
            return nullptr;
        }
        return AssetManager::get().getAsset<Texture>(mTextureId);
    }

    void RendererComponent::setColor(const glm::uvec3& color) {
        mColor = color;
    }

    void RendererComponent::setMesh(const std::string& modelId, uint32_t meshIndex) {
        mModelId = modelId;
        mMeshIdx = meshIndex;
    }

    std::shared_ptr<Mesh> RendererComponent::getMesh() const {
        if (mModelId.empty() || !mMeshIdx.has_value()) {
            return nullptr;
        }

        auto model = AssetManager::get().getAsset<Model>(mModelId);
        if (!model || mMeshIdx.value() >= model->getMeshes().size()) {
            return nullptr;
        }

        return model->getMesh(mMeshIdx.value());
    }

    void RendererComponent::deserialize(const pugi::xml_node& node) {
        mModelId.clear();
        mTextureId.clear();
        mMeshIdx.reset();

        if (auto attr = node.attribute("model")) {
            mModelId = attr.as_string();
        }

        if (auto attr = node.attribute("texture")) {
            mTextureId = attr.as_string();
        }

        if (auto attr = node.attribute("color")) {
            std::string colorStr = attr.as_string();
            std::stringstream ss(colorStr);
            char comma1, comma2;
            ss >> mColor.r >> comma1 >> mColor.g >> comma2 >> mColor.b;
        }

        if (auto attr = node.attribute("mesh")) {
            mMeshIdx = attr.as_uint();
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

    void RendererComponent::serialize(pugi::xml_node& node) const {
        if (!mModelId.empty()) {
            node.append_attribute("model") = mModelId.c_str();
        }

        if (!mTextureId.empty()) {
            node.append_attribute("texture") = mTextureId.c_str();
        }

        std::ostringstream colorStr;
        colorStr << mColor.r << "," << mColor.g << "," << mColor.b;
        node.append_attribute("color") = colorStr.str().c_str();

        if (mMeshIdx.has_value()) {
            node.append_attribute("mesh") = mMeshIdx.value();
        }

        node.append_attribute("active") = isActive();
    }

}
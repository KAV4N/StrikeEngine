#include "strikepch.h"
#include "RendererComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace Strike {

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
            unsigned int r = 255, g = 255, b = 255;
            if (sscanf(attr.as_string(), "%u,%u,%u", &r, &g, &b) == 3) {
                setColor(glm::uvec3(r, g, b));
            }
        }


        if (auto attr = node.attribute("mesh")) {
            mMeshIdx = attr.as_uint();
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }
}
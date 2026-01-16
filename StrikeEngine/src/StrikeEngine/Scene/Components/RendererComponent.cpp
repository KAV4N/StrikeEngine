#include "strikepch.h"
#include "RendererComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    REGISTER_COMPONENT(RendererComponent)

    RendererComponent::RendererComponent()
        : mColor(255.0f, 255.0f, 255.0f, 1.0f) // Default white color, no blending
    {}

    bool RendererComponent::setModel(const std::string& modelId) {
        mModel = AssetManager::get().getModel(modelId);
        mMeshIdx.reset();
        return mModel != nullptr;
    }

    void RendererComponent::removeModel() {
        mModel.reset();
        mMeshIdx.reset();
    }

    bool RendererComponent::hasModel() const {
        return mModel != nullptr;
    }

    std::shared_ptr<Model> RendererComponent::getModel() const {
        return mModel;
    }

    bool RendererComponent::setTexture(const std::string& textureId) {
        mTexture = AssetManager::get().getTexture(textureId);
        return mTexture != nullptr;
    }

    void RendererComponent::removeTexture() {
        mTexture.reset();
    }

    bool RendererComponent::hasTexture() const {
        return mTexture != nullptr;
    }

    std::shared_ptr<Texture> RendererComponent::getTexture() const {
        return mTexture;
    }

    void RendererComponent::setColor(const glm::vec4& color) {
        mColor = color;
    }

    bool RendererComponent::setMesh(const std::string& modelId, uint32_t meshIndex) {
        auto model = AssetManager::get().getModel(modelId);
        if (!model || meshIndex >= model->getMeshes().size()) {
            return false;
        }

        mModel = model;
        mMeshIdx = meshIndex;
        return true;
    }

    std::shared_ptr<Mesh> RendererComponent::getMesh() const {
        if (!mModel || !mMeshIdx.has_value()) {
            return nullptr;
        }

        return mModel->getMesh(mMeshIdx.value());
    }

    void RendererComponent::deserialize(const pugi::xml_node& node) {
        mModel.reset();
        mTexture.reset();
        mMeshIdx.reset();

        if (auto attr = node.attribute("model")) {
            setModel(attr.as_string());
        }

        if (auto attr = node.attribute("texture")) {
            setTexture(attr.as_string());
        }

        if (auto attr = node.attribute("color")) {
            std::string colorStr = attr.as_string();
            std::stringstream ss(colorStr);
            char comma1, comma2, comma3;
            ss >> mColor.r >> comma1 >> mColor.g >> comma2 >> mColor.b >> comma3 >> mColor.a;
        }

        if (auto attr = node.attribute("mesh")) {
            uint32_t meshIndex = attr.as_uint();
            mMeshIdx = meshIndex;
            
            if (mModel && meshIndex >= mModel->getMeshes().size()) {
                mMeshIdx.reset();
            }
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

    void RendererComponent::serialize(pugi::xml_node& node) const {
        if (mModel) {
            node.append_attribute("model") = mModel->getId().c_str();
        }

        if (mTexture) {
            node.append_attribute("texture") = mTexture->getId().c_str();
        }

        std::ostringstream colorStr;
        colorStr << mColor.r << "," << mColor.g << "," << mColor.b << "," << mColor.a;
        node.append_attribute("color") = colorStr.str().c_str();

        if (mMeshIdx.has_value()) {
            node.append_attribute("meshIdx") = mMeshIdx.value();
        }

        node.append_attribute("active") = isActive();
    }

}
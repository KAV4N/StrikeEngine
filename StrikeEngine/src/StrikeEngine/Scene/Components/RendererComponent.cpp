#include "strikepch.h"
#include "RendererComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/AssetManager.h"


namespace StrikeEngine {
    REGISTER_COMPONENT(RendererComponent)

    RendererComponent::RendererComponent() {}

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

    bool RendererComponent::setMaterial(const std::string& materialId) {
        mMaterial = AssetManager::get().getMaterial(materialId);
        return mMaterial != nullptr;
    }


    void RendererComponent::removeMaterial() {
        mMaterial.reset();
    }

    bool RendererComponent::hasMaterial() const {
        return mMaterial != nullptr;
    }

    std::shared_ptr<Material> RendererComponent::getMaterial() const {
        return mMaterial;
    }

    bool RendererComponent::setMesh(const std::string& modelId,
                                uint32_t meshIndex,
                                const std::string& materialId)
    {
        auto model = AssetManager::get().getModel(modelId);
        if (!model || meshIndex >= model->getMeshes().size()) {
            return false;
        }

        if (!materialId.empty()) {
            auto material = AssetManager::get().getMaterial(materialId);
            if (!material) {
                return false;
            }
            mMaterial = material;
        } else {
            mMaterial.reset();
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
        // Clear previous state
        mModel.reset();
        mMaterial.reset();
        mMeshIdx.reset();

        // Load model
        if (auto attr = node.attribute("model")) {
            setModel(attr.as_string());
        }

        // Load material
        if (auto attr = node.attribute("material")) {
            setMaterial(attr.as_string());
        }

        // Load specific mesh index if specified
        if (auto attr = node.attribute("mesh")) {
            uint32_t meshIndex = attr.as_uint();
            mMeshIdx = meshIndex;
            
            // Validate the mesh index
            if (mModel && meshIndex >= mModel->getMeshes().size()) {
                mMeshIdx.reset();
            }
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

    void RendererComponent::serialize(pugi::xml_node& node) const {
        // Serialize model
        if (mModel) {
            node.append_attribute("model") = mModel->getId().c_str();
        }

        // Serialize material
        if (mMaterial) {
            node.append_attribute("material") = mMaterial->getId().c_str();
        }

        // Serialize specific mesh index if set
        if (mMeshIdx.has_value()) {
            node.append_attribute("meshIdx") = mMeshIdx.value();
        }

        node.append_attribute("active") = isActive();
    }

}
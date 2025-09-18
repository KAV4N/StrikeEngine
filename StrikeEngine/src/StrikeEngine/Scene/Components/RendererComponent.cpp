#include "RendererComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {

    // RendererComponent implementation
    RendererComponent::RendererComponent() {}

    RendererComponent::RendererComponent(const std::string& meshId) {
        setMeshId(meshId);
    }

    RendererComponent::RendererComponent(const std::string& meshId, const std::string& materialId) {
        setMeshId(meshId);
        setMaterial(0, materialId);
    }

    // Mesh management
    void RendererComponent::setMeshId(const std::string& meshId) {
        mMeshId = meshId;
        mMesh = AssetManager::get().getMesh(meshId);
    }

    void RendererComponent::removeMesh() {
        mMeshId.clear();
        mMesh.reset();
    }

    bool RendererComponent::hasMesh() const {
        return !mMeshId.empty() && mMesh != nullptr;
    }

    const std::string& RendererComponent::getMeshId() const {
        return mMeshId;
    }

    std::shared_ptr<Mesh> RendererComponent::getMesh() const {
        return mMesh;
    }

    // Material management
    void RendererComponent::setMaterial(uint32_t slot, const std::string& materialId) {
        mMaterials[slot] = materialId;
        mMaterialAssets[slot] = AssetManager::get().getMaterial(materialId);
    }

    void RendererComponent::addMaterials(const std::unordered_map<uint32_t, std::string>& materials) {
        for (const auto& [slot, materialId] : materials) {
            setMaterial(slot, materialId);
        }
    }

    void RendererComponent::removeMaterial(uint32_t slot) {
        mMaterials.erase(slot);
        mMaterialAssets.erase(slot);
    }

    void RendererComponent::clearMaterials() {
        mMaterials.clear();
        mMaterialAssets.clear();
    }

    bool RendererComponent::hasMaterial(uint32_t slot) const {
        return mMaterials.find(slot) != mMaterials.end() && mMaterialAssets.find(slot) != mMaterialAssets.end();
    }

    const std::string& RendererComponent::getMaterial(uint32_t slot) const {
        static const std::string empty;
        auto it = mMaterials.find(slot);
        return (it != mMaterials.end()) ? it->second : empty;
    }

    const std::unordered_map<uint32_t, std::string>& RendererComponent::getMaterials() const {
        return mMaterials;
    }

    std::shared_ptr<Material> RendererComponent::getMaterialAsset(uint32_t slot) const {
        auto it = mMaterialAssets.find(slot);
        return (it != mMaterialAssets.end()) ? it->second : nullptr;
    }

    const std::unordered_map<uint32_t, std::shared_ptr<Material>>& RendererComponent::getMaterialAssets() const {
        return mMaterialAssets;
    }

    void RendererComponent::deserialize(const pugi::xml_node& node) {
        // Mesh ID
        if (auto attr = node.attribute("meshId")) {
            setMeshId(attr.as_string());
        }

        // Default material ID
        if (auto attr = node.attribute("materialId")) {
            setMaterial(0, attr.as_string());
        }

        // Material slots
        for (pugi::xml_node materialNode : node.children("material")) {
            int slot = materialNode.attribute("slot").as_int(0); // default to slot 0 if missing
            std::string matId = materialNode.attribute("materialId").as_string();
            if (!matId.empty()) {
                setMaterial(slot, matId);
            }
        }
    }


    void RendererComponent::serialize(pugi::xml_node& node) const {
        if (!mMeshId.empty()) {
            node.append_attribute("meshId") = mMeshId.c_str();
        }

        const auto& materials = mMaterials;
        if (materials.find(0) != materials.end()) {
            node.append_attribute("materialId") = getMaterial(0).c_str();
        }

        for (const auto& [slot, materialId] : materials) {
            if (slot != 0) {
                pugi::xml_node materialNode = node.append_child("material");
                materialNode.append_attribute("slot") = slot;
                materialNode.append_attribute("materialId") = materialId.c_str();
            }
        }
    }

}
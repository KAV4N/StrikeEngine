#include "RendererComponent.h"

namespace StrikeEngine {

    RendererComponentSerializer::RendererComponentSerializer(RendererComponent& component) : mComponent(component) {}

    void RendererComponentSerializer::deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
        auto meshIdIt = attributes.find("meshId");
        if (meshIdIt != attributes.end()) {
            mComponent.setMeshId(meshIdIt->second);
        }

        auto materialIdIt = attributes.find("materialId");
        if (materialIdIt != attributes.end()) {
            mComponent.setMaterial(0, materialIdIt->second);
        }

        for (pugi::xml_node materialNode : node.children("material")) {
            int slot = materialNode.attribute("slot").as_int(0);
            std::string matId = materialNode.attribute("materialId").as_string();
            if (!matId.empty()) {
                mComponent.setMaterial(slot, matId);
            }
        }
    }

    void RendererComponentSerializer::serialize(pugi::xml_node& node) const {
        if (!mComponent.getMeshId().empty()) {
            node.append_attribute("meshId") = mComponent.getMeshId().c_str();
        }

        const auto& materials = mComponent.getMaterials();
        if (materials.find(0) != materials.end()) {
            node.append_attribute("materialId") = mComponent.getMaterial(0).c_str();
        }

        for (const auto& [slot, materialId] : materials) {
            if (slot != 0) {
                pugi::xml_node materialNode = node.append_child("material");
                materialNode.append_attribute("slot") = slot;
                materialNode.append_attribute("materialId") = materialId.c_str();
            }
        }
    }

    RendererComponent::RendererComponent() {}

    RendererComponent::RendererComponent(const std::string& meshId)
        : mMeshId(meshId) {
    }

    RendererComponent::RendererComponent(const std::string& meshId, const std::string& materialId)
        : mMeshId(meshId) {
        mMaterials[0] = materialId;
    }

    const std::string& RendererComponent::getTypeName() const {
        static const std::string typeName = "renderer";
        return typeName;
    }


    void RendererComponent::setMeshId(const std::string& meshId) {
        mMeshId = meshId;
    }

    const std::string& RendererComponent::getMeshId() const {
        return mMeshId;
    }

    void RendererComponent::setMaterial(uint32_t slot, const std::string& materialId) {
        mMaterials[slot] = materialId;
    }

    const std::string& RendererComponent::getMaterial(uint32_t slot) const {
        static const std::string empty;
        auto it = mMaterials.find(slot);
        return (it != mMaterials.end()) ? it->second : empty;
    }

    const std::unordered_map<uint32_t, std::string>& RendererComponent::getMaterials() const {
        return mMaterials;
    }
}
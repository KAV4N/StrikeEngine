#include "RendererComponent.h"

namespace StrikeEngine {

    RendererComponent::RendererComponent(const std::string& meshId) : m_meshId(meshId) {}

    RendererComponent::RendererComponent(const std::string& meshId, const std::string& materialId)
        : m_meshId(meshId) {
        m_materials[0] = materialId;
    }

    void RendererComponent::SetAttributes(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
        auto meshIdIt = attributes.find("meshId");
        if (meshIdIt != attributes.end()) {
            m_meshId = meshIdIt->second;
        }

        auto materialIdIt = attributes.find("materialId");
        if (materialIdIt != attributes.end()) {
            m_materials[0] = materialIdIt->second;
        }

        // Handle additional material nodes
        for (pugi::xml_node materialNode : node.children("material")) {
            int slot = materialNode.attribute("slot").as_int(0);
            std::string matId = materialNode.attribute("materialId").as_string();
            if (!matId.empty()) {
                m_materials[slot] = matId;
            }
        }
    }

    void RendererComponent::SetMaterial(int slot, const std::string& materialId) {
        m_materials[slot] = materialId;
    }

    const std::string& RendererComponent::GetMaterial(int slot) const {
        static const std::string empty;
        auto it = m_materials.find(slot);
        return (it != m_materials.end()) ? it->second : empty;
    }

}
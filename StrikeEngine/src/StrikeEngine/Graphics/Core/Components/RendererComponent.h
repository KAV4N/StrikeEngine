#pragma once
#include "Component.h"
#include <string>
#include <unordered_map>
#include <pugixml.hpp>

namespace StrikeEngine {
    class RendererComponent : public Component {
    public:
        RendererComponent() = default;
        RendererComponent(const std::string& meshId);
        RendererComponent(const std::string& meshId, const std::string& materialId);

        const std::string& GetTypeName() const override {
            static const std::string typeName = "renderer";
            return typeName;
        }

        void SetAttributes(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) override;

        void SetMeshId(const std::string& meshId) { m_meshId = meshId; }
        const std::string& GetMeshId() const { return m_meshId; }

        void SetMaterial(int slot, const std::string& materialId);
        const std::string& GetMaterial(int slot) const;
        const std::unordered_map<int, std::string>& GetMaterials() const { return m_materials; }

    private:
        std::string m_meshId;
        std::unordered_map<int, std::string> m_materials;
    };
}
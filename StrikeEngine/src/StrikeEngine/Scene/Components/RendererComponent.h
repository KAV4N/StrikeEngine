#pragma once

#include "Component.h"
#include <string>
#include <unordered_map>
#include <pugixml.hpp>

namespace StrikeEngine {
    
    class RendererComponentSerializer;

    class RendererComponent : public Component {
    public:
        RendererComponent();
        RendererComponent(const std::string& meshId);
        RendererComponent(const std::string& meshId, const std::string& materialId);

        const std::string& getTypeName() const override;
        void deserializeAttributes(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) override;
        void serializeAttributes(pugi::xml_node& node) const override;

        void setMeshId(const std::string& meshId);
        const std::string& getMeshId() const;

        void setMaterial(uint32_t slot, const std::string& materialId);
        const std::string& getMaterial(uint32_t slot) const;
        const std::unordered_map<uint32_t, std::string>& getMaterials() const;

    private:
        std::string mMeshId;
        std::unordered_map<uint32_t, std::string> mMaterials;
    };

    class RendererComponentSerializer : public ComponentSerializer {
    public:
        RendererComponentSerializer(RendererComponent& component);
        void deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

    private:
        RendererComponent& mComponent;
    };
}
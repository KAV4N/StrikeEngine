#pragma once
#include "Component.h"

#include <string>
#include <unordered_map>
#include <pugixml.hpp>
#include <memory>

namespace StrikeEngine {

    class RendererComponent : public Component {
    public:
        RendererComponent();
        RendererComponent(const std::string& meshId);
        RendererComponent(const std::string& meshId, const std::string& materialId);

        // Static type name for registration
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "renderer";
            return typeName;
        }

        // Virtual method implementation
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        // Serialization methods
        void deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        void setMeshId(const std::string& meshId);
        const std::string& getMeshId() const;
        void setMaterial(uint32_t slot, const std::string& materialId);
        const std::string& getMaterial(uint32_t slot) const;
        const std::unordered_map<uint32_t, std::string>& getMaterials() const;

    private:
        std::string mMeshId;
        std::unordered_map<uint32_t, std::string> mMaterials;
    };

}
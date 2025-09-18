#pragma once
#include "Component.h"
#include "StrikeEngine/Asset/Types/Mesh.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/AssetManager.h"

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
        void deserialize( const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Mesh management
        void setMeshId(const std::string& meshId);
        void removeMesh();
        bool hasMesh() const;
        const std::string& getMeshId() const;
        std::shared_ptr<Mesh> getMesh() const;

        // Material management
        void setMaterial(uint32_t slot, const std::string& materialId);
        void addMaterials(const std::unordered_map<uint32_t, std::string>& materials);
        void removeMaterial(uint32_t slot);
        void clearMaterials();
        bool hasMaterial(uint32_t slot) const;
        const std::string& getMaterial(uint32_t slot) const;
        const std::unordered_map<uint32_t, std::string>& getMaterials() const;
        std::shared_ptr<Material> getMaterialAsset(uint32_t slot) const;
        const std::unordered_map<uint32_t, std::shared_ptr<Material>>& getMaterialAssets() const;

    private:
        std::string mMeshId;
        std::shared_ptr<Mesh> mMesh;
        std::unordered_map<uint32_t, std::string> mMaterials;
        std::unordered_map<uint32_t, std::shared_ptr<Material>> mMaterialAssets;
    };

}
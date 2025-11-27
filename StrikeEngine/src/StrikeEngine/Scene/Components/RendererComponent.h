#pragma once
#include "Component.h"


#include <string>
#include <pugixml.hpp>
#include <memory>
#include <optional>

namespace StrikeEngine {
    class Mesh;
    class Model;
    class Material;
    class AssetManager;

    class RendererComponent : public Component {
    public:
        RendererComponent();

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
        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Model management - render entire model with single material
        void setModel(const std::string& modelId);
        void removeModel();
        bool hasModel() const;
        std::shared_ptr<Model> getModel() const;

        // Material management
        void setMaterial(const std::string& materialId);
        void removeMaterial();
        bool hasMaterial() const;
        std::shared_ptr<Material> getMaterial() const;

        // Specific mesh rendering - render only one mesh from the model
        void setMesh(const std::string& modelId, uint32_t meshIndex, const std::string& materialId = "");
        std::shared_ptr<Mesh> getMesh() const;
        
        // Getters for specific mesh configuration
        bool hasMesh() const { return mMeshIdx.has_value(); }
        std::optional<uint32_t> getMeshIdx() const { return mMeshIdx; }

    private:
        std::shared_ptr<Model> mModel;
        std::shared_ptr<Material> mMaterial;
        
        // Optional: if set, only render this specific mesh from the model
        std::optional<uint32_t> mMeshIdx;
    };

}
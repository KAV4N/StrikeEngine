#pragma once

#include "Component.h"

#include <string>
#include <pugixml.hpp>
#include <memory>
#include <optional>
#include <glm/glm.hpp>

namespace StrikeEngine {

    class Mesh;
    class Model;
    class Texture;
    class AssetManager;

    class RendererComponent : public Component {
    public:
        RendererComponent();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "renderer";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Model management
        bool setModel(const std::string& modelId);
        void removeModel();
        bool hasModel() const;
        std::shared_ptr<Model> getModel() const;

        // Texture management
        bool setTexture(const std::string& textureId);
        void removeTexture();
        bool hasTexture() const;
        std::shared_ptr<Texture> getTexture() const;

        // Color management (0-255 RGB range)
        void setColor(const glm::vec4& color);
        glm::vec4 getColor() const { return mColor; }

        // Specific mesh rendering
        bool setMesh(const std::string& modelId, uint32_t meshIndex);
        std::shared_ptr<Mesh> getMesh() const;
        
        bool hasMesh() const { return mMeshIdx.has_value(); }
        std::optional<uint32_t> getMeshIdx() const { return mMeshIdx; }

    private:
        std::shared_ptr<Model> mModel;
        std::shared_ptr<Texture> mTexture;
        glm::vec4 mColor; // RGB in 0-255 range, A (blend) in 0-1 range
        
        std::optional<uint32_t> mMeshIdx;
    };

}
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

        // Model management
        void setModel(const std::string& modelId);
        void removeModel();
        bool hasModel() const;
        const std::string& getModelId() const { return mModelId; }
        std::shared_ptr<Model> getModel() const;

        // Texture management
        void setTexture(const std::string& textureId);
        void removeTexture();
        bool hasTexture() const;
        const std::string& getTextureId() const { return mTextureId; }
        std::shared_ptr<Texture> getTexture() const;

        // Color management (0-255 RGB range)
        void setColor(const glm::uvec3& color);
        glm::uvec3 getColor() const { return mColor; }

        // Specific mesh rendering
        void setMesh(const std::string& modelId, uint32_t meshIndex);
        std::shared_ptr<Mesh> getMesh() const;
        
        bool hasMesh() const { return mMeshIdx.has_value(); }
        std::optional<uint32_t> getMeshIdx() const { return mMeshIdx; }

    private:
        std::string mModelId;
        std::string mTextureId;
        glm::uvec3 mColor; 
        std::optional<uint32_t> mMeshIdx;
    };

}
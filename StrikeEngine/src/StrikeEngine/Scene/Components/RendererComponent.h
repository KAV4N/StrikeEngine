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

    /**
     * @brief Component for rendering 3D models and meshes.
     * 
     * The RendererComponent manages the visual representation of an entity,
     * including model/mesh assignment, texture mapping, and color tinting.
     * 
     * @note Colors are stored as RGB integers in the range 0-255
     */
    class RendererComponent : public Component {
    public:
        /**
         * @brief Default constructor
         */
        RendererComponent();

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "renderer"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "renderer";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "renderer"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Deserialize renderer data from XML node
         * 
         * @param node The XML node containing renderer configuration
         */
        void deserialize(const pugi::xml_node& node) override;

        /**
         * @brief Set the model to render
         * 
         * @param modelId Asset ID of the model to use
         */
        void setModel(const std::string& modelId);
        
        /**
         * @brief Remove the currently assigned model
         */
        void removeModel();
        
        /**
         * @brief Check if a model is assigned
         * 
         * @return True if a model is assigned, false otherwise
         */
        bool hasModel() const;
        
        /**
         * @brief Get the ID of the currently assigned model
         * 
         * @return Constant reference to the model asset ID
         */
        const std::string& getModelId() const { return mModelId; }
        
        /**
         * @brief Get the model resource object
         * 
         * @return Shared pointer to the Model, or nullptr if not set
         */
        std::shared_ptr<Model> getModel() const;

        /**
         * @brief Set the texture to apply to the model
         * 
         * @param textureId Asset ID of the texture to use
         */
        void setTexture(const std::string& textureId);
        
        /**
         * @brief Remove the currently assigned texture
         */
        void removeTexture();
        
        /**
         * @brief Check if a texture is assigned
         * 
         * @return True if a texture is assigned, false otherwise
         */
        bool hasTexture() const;
        
        /**
         * @brief Get the ID of the currently assigned texture
         * 
         * @return Constant reference to the texture asset ID
         */
        const std::string& getTextureId() const { return mTextureId; }
        
        /**
         * @brief Get the texture resource object
         * 
         * @return Shared pointer to the Texture, or nullptr if not set
         */
        std::shared_ptr<Texture> getTexture() const;

        /**
         * @brief Set the color tint for rendering
         * 
         * @param color RGB color values (0-255 per channel)
         */
        void setColor(const glm::uvec3& color);
        
        /**
         * @brief Get the color tint
         * 
         * @return RGB color values (0-255)
         */
        glm::uvec3 getColor() const { return mColor; }

        /**
         * @brief Set a specific mesh from a model to render
         * 
         * @param modelId Asset ID of the model
         * @param meshIndex Index of the mesh within the model
         * @note Use this to render only a single mesh from a multi-mesh model
         */
        void setMesh(const std::string& modelId, uint32_t meshIndex);
        
        /**
         * @brief Get the currently selected mesh
         * 
         * @return Shared pointer to the Mesh, or nullptr if no specific mesh is set
         */
        std::shared_ptr<Mesh> getMesh() const;
        
        /**
         * @brief Check if a specific mesh is selected
         * 
         * @return True if a mesh index is set, false otherwise
         */
        bool hasMesh() const { return mMeshIdx.has_value(); }
        
        /**
         * @brief Get the index of the selected mesh
         * 
         * @return Optional containing the mesh index, or empty if not set
         */
        std::optional<uint32_t> getMeshIdx() const { return mMeshIdx; }

    private:
        std::string mModelId;              ///< Model asset ID
        std::string mTextureId;            ///< Texture asset ID
        glm::uvec3 mColor;                 ///< RGB color tint (0-255)
        std::optional<uint32_t> mMeshIdx;  ///< Optional specific mesh index
    };

}

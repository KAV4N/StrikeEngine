#pragma once

#include "AssetLoader.h"
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <pugixml.hpp>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Strike {

    class Asset;
    class Model;
    class Mesh;
    struct Vertex;
    struct Bounds;

    class ModelLoader : public AssetLoader {
    public:
        ModelLoader();
        ~ModelLoader() = default;

        /**
         * @brief Load an asset from an XML node.
         * @param node The XML node containing asset data.
         * @param basePath The base path to resolve relative asset paths.
         * @return Shared pointer to the loaded asset.
         */
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        std::shared_ptr<Model> loadModelWithAssimp(const std::string& id, const std::filesystem::path& filePath);
        std::shared_ptr<Mesh> processMesh(aiMesh* aiMesh, const aiScene* scene);
        std::vector<Vertex> processVertices(aiMesh* aiMesh);
        std::vector<uint32_t> processIndices(aiMesh* aiMesh);
        Bounds calculateBounds(const std::vector<Vertex>& vertices);
        void processNode(aiNode* node, const aiScene* scene, Model* model);
    };
}
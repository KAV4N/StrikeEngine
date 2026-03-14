#include "strikepch.h"

#include "ModelLoader.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace Strike {

    ModelLoader::ModelLoader() : AssetLoader(Model::getStaticTypeName()) {}

    std::shared_ptr<Asset> ModelLoader::loadAssetInternal(const std::string& id,
                                                          const std::filesystem::path& filePath,
                                                          bool async) {
        return loadModelWithAssimp(id, filePath);
    }

    void ModelLoader::swapData(std::shared_ptr<Asset> placeholder,
                               const std::shared_ptr<Asset> loaded) {
        auto placeholderAsset = std::dynamic_pointer_cast<Model>(placeholder);
        auto loadedAsset      = std::dynamic_pointer_cast<Model>(loaded);
        *placeholderAsset = std::move(*loadedAsset);
    }

    std::shared_ptr<Model> ModelLoader::loadModelWithAssimp(const std::string& id,
                                                            const std::filesystem::path& filePath) {
        auto asset = std::make_shared<Model>(id, filePath);
        asset->setState(AssetState::Loading);

        if (!std::filesystem::exists(filePath)) {
            STRIKE_CORE_ERROR("Model file does not exist: {}", filePath.string());
            asset->setState(AssetState::Failed);
            return asset;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath.string(),
            aiProcess_Triangulate           |
            aiProcess_GenSmoothNormals      |
            aiProcess_CalcTangentSpace      |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality  |
            aiProcess_ValidateDataStructure);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            STRIKE_CORE_ERROR("Assimp failed to load model {}: {}",
                              filePath.string(), importer.GetErrorString());
            asset->setState(AssetState::Failed);
            return asset;
        }

        try {
            processNode(scene->mRootNode, scene, asset.get());
            return asset;
        }
        catch (const std::exception& e) {
            STRIKE_CORE_ERROR("Error processing model {}: {}", filePath.string(), e.what());
            asset->setState(AssetState::Failed);
            return asset;
        }
    }

    void ModelLoader::processNode(aiNode* node, const aiScene* scene, Model* model) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model->addMesh(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, model);
        }
    }

    std::shared_ptr<Mesh> ModelLoader::processMesh(aiMesh* aiMesh, const aiScene* scene) {
        auto mesh = std::make_shared<Mesh>();

        mesh->setVertices(processVertices(aiMesh));
        mesh->setIndices(processIndices(aiMesh));
        mesh->setBounds(calculateBounds(mesh->getVertices()));

        return mesh;
    }

    std::vector<Vertex> ModelLoader::processVertices(aiMesh* aiMesh) {
        std::vector<Vertex> vertices;
        vertices.reserve(aiMesh->mNumVertices);

        for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.position = { aiMesh->mVertices[i].x,
                                aiMesh->mVertices[i].y,
                                aiMesh->mVertices[i].z };

            vertex.normal = aiMesh->HasNormals()
                ? glm::vec3(aiMesh->mNormals[i].x,
                            aiMesh->mNormals[i].y,
                            aiMesh->mNormals[i].z)
                : glm::vec3(0.0f, 1.0f, 0.0f);

            vertex.texCoord = aiMesh->HasTextureCoords(0)
                ? glm::vec2(aiMesh->mTextureCoords[0][i].x,
                            aiMesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            vertex.tangent = aiMesh->HasTangentsAndBitangents()
                ? glm::vec3(aiMesh->mTangents[i].x,
                            aiMesh->mTangents[i].y,
                            aiMesh->mTangents[i].z)
                : glm::vec3(1.0f, 0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        return vertices;
    }

    std::vector<uint32_t> ModelLoader::processIndices(aiMesh* aiMesh) {
        std::vector<uint32_t> indices;

        for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
            const aiFace& face = aiMesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return indices;
    }

    Bounds ModelLoader::calculateBounds(const std::vector<Vertex>& vertices) {
        if (vertices.empty()) return Bounds();

        Bounds bounds(vertices[0].position);
        for (const auto& vertex : vertices) {
            bounds.unionBounds(Bounds(vertex.position));
        }

        return bounds;
    }

    std::shared_ptr<Asset> ModelLoader::loadFromNode(const pugi::xml_node& node,
                                                     const std::filesystem::path& basePath) {
        return loadFromNodeInternal<Model>(node, basePath);
    }
}
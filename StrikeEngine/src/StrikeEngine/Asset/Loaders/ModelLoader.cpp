#include "strikepch.h"

#include "ModelLoader.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    ModelLoader::ModelLoader() : AssetLoader(Model::getStaticTypeName()) {}

    std::shared_ptr<Asset> ModelLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& filePath, bool async) {
        auto model = loadModelWithAssimp(id, filePath);
        if (!model) {
            throw std::runtime_error("Failed to load model: " + filePath.string());
        }
        return model;
    }

    std::shared_ptr<Asset> ModelLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("id").as_string();
        std::filesystem::path src = node.attribute("src").as_string();
        std::string srcString = src.string();

        bool async = node.attribute("async").as_bool();
        
        if (assetId.empty() || src.empty()) {
            throw std::runtime_error("Invalid model node: missing assetId or src attribute");
        }

        src = resolvePath(src, basePath);

        if (async) 
            return AssetManager::get().loadModelAsync(assetId, src);
        else 
            return AssetManager::get().loadModel(assetId, src);
    }

    void ModelLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderAsset = std::dynamic_pointer_cast<Model>(placeholder);
        auto loadedAsset = std::dynamic_pointer_cast<Model>(loaded);
        *placeholderAsset = std::move(*loadedAsset);
    }

    std::shared_ptr<Model> ModelLoader::loadModelWithAssimp(const std::string& id, const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error("Model file does not exist: " + filePath.string());
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath.string(), 
            aiProcess_Triangulate | 
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_ValidateDataStructure);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error("Assimp failed to load model: " + std::string(importer.GetErrorString()));
        }

        auto model = std::make_shared<Model>(id, addRootPrefix(filePath));
        
        try {
            processNode(scene->mRootNode, scene, model.get());
            return model;
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error processing model data from " + filePath.string() + ": " + e.what());
        }
    }

    void ModelLoader::processNode(aiNode* node, const aiScene* scene, Model* model) {
        // Process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
            auto mesh = processMesh(aiMesh, scene);
            model->addMesh(mesh);
        }
        
        // Then process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, model);
        }
    }

    std::shared_ptr<Mesh> ModelLoader::processMesh(aiMesh* aiMesh, const aiScene* scene) {
        auto mesh = std::make_shared<Mesh>();
        
        std::vector<Vertex> vertices = processVertices(aiMesh);
        std::vector<uint32_t> indices = processIndices(aiMesh);
        Bounds bounds = calculateBounds(vertices);
        
        mesh->setVertices(vertices);
        mesh->setIndices(indices);
        mesh->setBounds(bounds);
        
        return mesh;
    }

    std::vector<Vertex> ModelLoader::processVertices(aiMesh* aiMesh) {
        std::vector<Vertex> vertices;
        vertices.reserve(aiMesh->mNumVertices);

        for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
            Vertex vertex;
            
            // Position
            vertex.position.x = aiMesh->mVertices[i].x;
            vertex.position.y = aiMesh->mVertices[i].y;
            vertex.position.z = aiMesh->mVertices[i].z;
            
            // Normals
            if (aiMesh->HasNormals()) {
                vertex.normal.x = aiMesh->mNormals[i].x;
                vertex.normal.y = aiMesh->mNormals[i].y;
                vertex.normal.z = aiMesh->mNormals[i].z;
            } else {
                vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            
            // Texture coordinates
            if (aiMesh->HasTextureCoords(0)) {
                vertex.texCoord.x = aiMesh->mTextureCoords[0][i].x;
                vertex.texCoord.y = aiMesh->mTextureCoords[0][i].y;
            } else {
                vertex.texCoord = glm::vec2(0.0f);
            }
            
            // Tangents and bitangents
            if (aiMesh->HasTangentsAndBitangents()) {
                vertex.tangent.x = aiMesh->mTangents[i].x;
                vertex.tangent.y = aiMesh->mTangents[i].y;
                vertex.tangent.z = aiMesh->mTangents[i].z;
            } else {
                vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            
            vertices.push_back(vertex);
        }
        
        return vertices;
    }

    std::vector<uint32_t> ModelLoader::processIndices(aiMesh* aiMesh) {
        std::vector<uint32_t> indices;
        
        for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
            aiFace face = aiMesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        
        return indices;
    }

    Bounds ModelLoader::calculateBounds(const std::vector<Vertex>& vertices) {
        if (vertices.empty()) {
            return Bounds();
        }
        
        Bounds bounds(vertices[0].position);
        
        for (const auto& vertex : vertices) {
            bounds.unionBounds(Bounds(vertex.position));
        }
        
        return bounds;
    }

    std::shared_ptr<Asset> ModelLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Model>(id, path);
    }
}
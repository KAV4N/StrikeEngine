
#include "strikepch.h"
#include "ModelManager.h"
#include "StrikeEngine/Renderer/Core/Texture.h"
#include "StrikeEngine/Renderer/Core/Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace StrikeEngine {

    ModelManager* ModelManager::s_Instance = nullptr;

    void ModelManager::Create() {
        if (!s_Instance)
            s_Instance = new ModelManager();
    }

    ModelManager* ModelManager::Get() {
        return s_Instance;
    }

    void ModelManager::Destroy() {
        if (s_Instance) {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }

    ModelManager::ModelManager() {}

    ModelManager::~ModelManager() {
        Clear();
    }

    Model* ModelManager::LoadModel(const std::string& path) {
        auto it = m_Models.find(path);
        if (it != m_Models.end())
            return it->second;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            //std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        std::string directory = path.substr(0, path.find_last_of('/'));
        auto meshes = ProcessNode(scene->mRootNode, scene, directory);

        Model* model = new Model();
        for (auto mesh : meshes) {
            model->AddMesh(mesh);
        }

        m_Models[path] = model;
        return model;
    }

    Model* ModelManager::GetModel(const std::string& path) {
        auto it = m_Models.find(path);
        return (it != m_Models.end()) ? it->second : nullptr;
    }

    void ModelManager::Clear() {
        for (auto& pair : m_Models) {
            delete pair.second;
        }
        m_Models.clear();
    }

    std::vector<Mesh*> ModelManager::ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory) {
        std::vector<Mesh*> meshes;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Mesh* processedMesh = ProcessMesh(mesh, scene, directory);
            meshes.push_back(processedMesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            auto childMeshes = ProcessNode(node->mChildren[i], scene, directory);
            meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
        }

        return meshes;
    }

    Mesh* ModelManager::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
        std::vector<Mesh::Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;

        // Extract vertex data
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Mesh::Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.TexCoords = mesh->HasTextureCoords(0) ?
                glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) :
                glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // Extract index data
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process material
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return new Mesh(vertices, indices, textures);
    }

    std::vector<Texture*> ModelManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory) {
        std::vector<Texture*> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string fullPath = directory + '/' + str.C_Str();
            bool skip = false;

            for (auto& texture : textures) {
                if (texture->GetPath() == fullPath) {
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                Texture* texture = new Texture(fullPath);
                textures.push_back(texture);
            }
        }
        return textures;
    }


    // Define the CreateSphereMesh, CreateCuboidMesh, CreatePlaneMesh, CreateCylinderMesh methods here...

    // SetupMeshBuffers method to setup VAO, VBO, EBO for the meshes...
}

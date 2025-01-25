#include "strikepch.h"
#include "ModelManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace StrikeEngine {

    ModelManager* ModelManager::s_Instance = nullptr;

    ModelManager* ModelManager::Create() {
        if (!s_Instance) {
            s_Instance = new ModelManager();
        }
        return s_Instance;
    }


    ModelManager* ModelManager::Get() {
        return s_Instance;
    }

    bool ModelManager::LoadModel(const std::string& path, const std::string& name,
        std::shared_ptr<Material> material) {

        for (const auto& pair : m_Models) {
            const ModelKey& existingKey = pair.first;
            if (existingKey.name == name || existingKey.path == path) {
                return false;
            }
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            return false;
        }

        auto model = std::make_shared<Model>();
        ProcessNode(scene->mRootNode, scene, material, model.get());
        m_Models[{path, name}] = model;
        return true;
    }

    bool ModelManager::LoadMesh(const std::string& name, std::shared_ptr<Mesh> mesh) {
        if (m_Meshes.find(name) != m_Meshes.end()) {
            return false;
        }
        m_Meshes[name] = mesh;
        return true;
    }

    void ModelManager::UnloadModel(const std::string& name) {
        for (auto it = m_Models.begin(); it != m_Models.end(); ) {
            if (it->first.name == name) {
                it = m_Models.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void ModelManager::UnloadMesh(const std::string& name) {
        m_Meshes.erase(name);
    }

    std::shared_ptr<Model> ModelManager::GetModel(const std::string& name) const {
        for (const auto& pair : m_Models) {
            if (pair.first.name == name) {
                return pair.second;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Mesh> ModelManager::GetMesh(const std::string& name) const {
        auto it = m_Meshes.find(name);
        return it != m_Meshes.end() ? it->second : nullptr;
    }

    void ModelManager::ProcessNode(aiNode* node, const aiScene* scene,
        std::shared_ptr<Material> material, Model* model) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
            model->AddMesh(ProcessMesh(ai_mesh, scene, material));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene, material, model);
        }
    }

    std::shared_ptr<Mesh> ModelManager::ProcessMesh(aiMesh* mesh, const aiScene* scene,
        std::shared_ptr<Material> material) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            };

            vertex.Normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };

            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };
            }
            else {
                vertex.TexCoords = { 0.0f, 0.0f };
            }

            vertices.push_back(vertex);
        }


        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_shared<Mesh>(vertices, indices, material);
    }

}
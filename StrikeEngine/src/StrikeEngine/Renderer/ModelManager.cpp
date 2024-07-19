#include "strikepch.h"
#include "ModelManager.h"
#include "StrikeEngine/Renderer/Texture.h"
#include "StrikeEngine/Renderer/Model.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace StrikeEngine {

    ModelManager* ModelManager::s_Instance = nullptr;

    ModelManager::ModelManager() {}

    ModelManager::~ModelManager() {
        Clear();
    }

    Model* ModelManager::LoadModel(const std::string& path) {
        if (m_Models.find(path) != m_Models.end())
            return m_Models[path];

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        std::string directory = path.substr(0, path.find_last_of('/'));
        auto parts = ProcessNode(scene->mRootNode, scene, directory);

        Model* model = new Model();
        for (auto part : parts) {
            model->AddPart(part);
        }

        m_Models[path] = model;
        return model;
    }

    Model* ModelManager::GetModel(const std::string& path) {
        if (m_Models.find(path) != m_Models.end())
            return m_Models[path];
        return nullptr;
    }

    void ModelManager::Clear() {
        for (auto& pair : m_Models) {
            delete pair.second;
        }
        m_Models.clear();
    }

    std::vector<ModelPart*> ModelManager::ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory) {
        std::vector<ModelPart*> parts;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ModelPart* part = ProcessMesh(mesh, scene, directory);
            parts.push_back(part);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            auto childParts = ProcessNode(node->mChildren[i], scene, directory);
            parts.insert(parts.end(), childParts.begin(), childParts.end());
        }

        return parts;
    }

    ModelPart* ModelManager::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Position
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            // Normals
            if (mesh->HasNormals()) {
                vertices.push_back(mesh->mNormals[i].x);
                vertices.push_back(mesh->mNormals[i].y);
                vertices.push_back(mesh->mNormals[i].z);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            // Texture coordinates
            if (mesh->HasTextureCoords(0)) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        unsigned int vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Vertex normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Vertex texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        ModelPart* modelPart = new ModelPart(vao, indices.size(), vbo, ebo);

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        auto diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        modelPart->AddTextures(diffuseTextures);

        auto specularTextures = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        modelPart->AddTextures(specularTextures);

        auto normalTextures = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", directory);
        modelPart->AddTextures(normalTextures);

        auto heightTextures = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height", directory);
        modelPart->AddTextures(heightTextures);

        return modelPart;
    }


    std::vector<Texture*> ModelManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory) {
        std::vector<Texture*> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string fullPath = directory + '/' + std::string(str.C_Str());

            bool skip = false;
            for (auto& texture : textures) {
                if (texture->GetPath() == fullPath) {
                    textures.push_back(texture);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                Texture* texture = new Texture(fullPath);
                texture->SetType(typeName);
                textures.push_back(texture);
            }
        }
        return textures;
    }

}

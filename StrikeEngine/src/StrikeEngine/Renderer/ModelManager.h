#pragma once

#include "StrikeEngine/Renderer/Model.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <assimp/scene.h>

namespace StrikeEngine {

    class ModelManager
    {
    public:
        static void Create();
        static ModelManager* Get();
        static void Destroy();

        Model* LoadModel(const std::string& path);
        Model* GetModel(const std::string& path);
        void Clear();

    private:
        ModelManager();
        ~ModelManager();

        std::vector<ModelPart*> ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory);
        ModelPart* ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
        void ExtractMeshData(aiMesh* mesh, std::vector<float>& vertices, std::vector<unsigned int>& indices);
        std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);

        static ModelManager* s_Instance;
        std::unordered_map<std::string, Model*> m_Models;

        void SetupMeshBuffers(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, unsigned int& vao, unsigned int& vbo, unsigned int& ebo);
        Material ExtractMaterial(aiMaterial* material);
    };

}

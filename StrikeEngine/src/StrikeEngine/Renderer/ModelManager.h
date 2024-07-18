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
        static void Create() {
            if (!s_Instance)
                s_Instance = new ModelManager();
        }

        static ModelManager* Get() {
            return s_Instance;
        }

        static void Destroy() {
            if (s_Instance) {
                delete s_Instance;
                s_Instance = nullptr;
            }
        }

        Model* LoadModel(const std::string& path);
        Model* GetModel(const std::string& path);
        void Clear();

    private:
        ModelManager();
        ~ModelManager();

        std::vector<ModelPart*> ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory);
        ModelPart* ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
        std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);

        static ModelManager* s_Instance;
        std::unordered_map<std::string, Model*> m_Models;
    };

}

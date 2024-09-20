
#pragma once

#include "StrikeEngine/Graphics/Core/Model.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <assimp/scene.h>

namespace StrikeEngine {

    class ModelManager {
    public:
        static void Create();
        static ModelManager* Get();
        static void Destroy();

        Model* LoadModel(const std::string& path);
        Model* GetModel(const std::string& path);
        void Clear();

        Model* CreatePlane(float width, float height);
        Model* CreateCylinder(float radius, float height, unsigned int sectors);
        Model* CreateSphere(float radius, unsigned int rings, unsigned int sectors);
        Model* CreateCuboid(float width, float height, float depth);

    private:
        ModelManager();
        ~ModelManager();

        std::vector<Mesh*> ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory);
        Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
        std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);


        static ModelManager* s_Instance;
        std::unordered_map<std::string, Model*> m_Models;
    };
}

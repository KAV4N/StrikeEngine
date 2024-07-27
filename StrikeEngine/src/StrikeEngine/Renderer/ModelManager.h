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

        Model* CreatePlane(float width, float height);
        Model* CreateCylinder(float radius, float height, unsigned int sectors);
        Model* CreateSphere(float radius, unsigned int rings, unsigned int sectors);
        Model* CreateCuboid(float width, float height, float depth);


    private:
        ModelManager();
        ~ModelManager();

        std::vector<ModelPart*> ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory);
        ModelPart* ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
        void ExtractMeshData(aiMesh* mesh, std::vector<float>& vertices, std::vector<unsigned int>& indices);
        std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);


        ModelPart* CreateSphereMesh(float radius, unsigned int rings, unsigned int sectors);
        ModelPart* CreateCuboidMesh(float width, float height, float depth);
        ModelPart* CreatePlaneMesh(float width, float height);
        ModelPart* CreateCylinderMesh(float radius, float height, unsigned int sectors);

        static ModelManager* s_Instance;
        std::unordered_map<std::string, Model*> m_Models;

        void SetupMeshBuffers(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, unsigned int& vao, unsigned int& vbo, unsigned int& ebo);
        Material ExtractMaterial(aiMaterial* material);
    };

}

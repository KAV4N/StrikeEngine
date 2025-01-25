#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <assimp/scene.h>
#include "StrikeEngine/Graphics/Core/Model.h"
#include "StrikeEngine/Graphics/Core/Mesh.h"

namespace StrikeEngine {

    class ModelManager {
    public:
        static ModelManager* Create();
        static ModelManager* Get();

        bool LoadModel(const std::string& path, const std::string& name,
            std::shared_ptr<Material> material = nullptr);
        void UnloadModel(const std::string& name);
        std::shared_ptr<Model> GetModel(const std::string& name) const;

        bool LoadMesh(const std::string& name, std::shared_ptr<Mesh> mesh);
        void UnloadMesh(const std::string& name);
        std::shared_ptr<Mesh> GetMesh(const std::string& name) const;

    private:
        ModelManager() = default;
        ~ModelManager() = default;

        struct ModelKey {
            std::string path;
            std::string name;

            bool operator==(const ModelKey& other) const {
                return path == other.path && name == other.name;
            }
        };

        struct KeyHash {
            size_t operator()(const ModelKey& k) const {
                return std::hash<std::string>()(k.path) ^
                    (std::hash<std::string>()(k.name) << 1);
            }
        };

        void ProcessNode(aiNode* node, const aiScene* scene,
            std::shared_ptr<Material> material, Model* model);
        std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene,
            std::shared_ptr<Material> material);
    private:
        static ModelManager* s_Instance;
        std::unordered_map<ModelKey, std::shared_ptr<Model>, KeyHash> m_Models;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;
    };

}
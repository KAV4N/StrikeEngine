#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "StrikeEngine/Graphics/Core/Mesh.h"

namespace StrikeEngine {

    struct MeshLoadSpec {
        //TODO: add default settings
    };

    struct OBJSpec : public MeshLoadSpec {
        bool CombineMeshes = false;
    };

    struct FBXSpec : public MeshLoadSpec {
        bool LoadSkeleton = true;
    };

    class MeshManager {
    public:
        static MeshManager* Create();
        static MeshManager* Get();

        bool LoadOBJ(const std::string& filepath, const OBJSpec& spec = OBJSpec());
        bool LoadFBX(const std::string& filepath, const FBXSpec& spec = FBXSpec());
        bool UnloadMesh(const std::string& meshName);
        bool RenameMesh(const std::string& oldName, const std::string& newName);
        std::shared_ptr<Mesh> GetMesh(const std::string& meshName);

    private:
        static MeshManager* s_Instance;
        MeshManager() = default;

        std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;

        void ProcessOBJNode(aiNode* node, const aiScene* scene, const OBJSpec& spec,
            std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
            std::unique_ptr<Skeleton>& skeleton);
        void ProcessFBXNode(aiNode* node, const aiScene* scene, const FBXSpec& spec,
            std::unordered_map<aiNode*, std::shared_ptr<Mesh>>& nodeMap);
        void ProcessMesh(aiMesh* mesh, const aiScene* scene,
            std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
            std::unique_ptr<Skeleton>& skeleton, bool loadSkeleton);
        std::string GenerateUniqueName(const std::string& baseName);
    };

}
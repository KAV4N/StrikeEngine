#include "MeshManager.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {
    MeshManager* MeshManager::s_Instance = nullptr;

    MeshManager* MeshManager::Create() {
        if (!s_Instance)
            s_Instance = new MeshManager();
        return s_Instance;
    }

    MeshManager* MeshManager::Get() {
        return s_Instance;
    }

    bool MeshManager::LoadOBJ(const std::string& filepath, const OBJSpec& spec) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath,
            aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return false;
        }

        if (spec.CombineMeshes) {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::unique_ptr<Skeleton> skeleton;

            ProcessOBJNode(scene->mRootNode, scene, spec, vertices, indices, skeleton);

            std::string meshName = GenerateUniqueName(scene->mRootNode->mName.C_Str());
            m_Meshes[meshName] = std::make_shared<Mesh>(vertices, indices, std::move(skeleton));
        }
        else {
            ProcessOBJNode(scene->mRootNode, scene, spec, std::vector<Vertex>(), std::vector<uint32_t>(), std::unique_ptr<Skeleton>());
        }

        return true;
    }

    bool MeshManager::LoadFBX(const std::string& filepath, const FBXSpec& spec) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath,
            aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return false;
        }

        std::unordered_map<aiNode*, std::shared_ptr<Mesh>> nodeMap;
        ProcessFBXNode(scene->mRootNode, scene, spec, nodeMap);

        return true;
    }

    void MeshManager::ProcessOBJNode(aiNode* node, const aiScene* scene, const OBJSpec& spec,
        std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
        std::unique_ptr<Skeleton>& skeleton) {

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            if (spec.CombineMeshes) {
                ProcessMesh(mesh, scene, vertices, indices, skeleton, false);
            }
            else {
                std::vector<Vertex> meshVertices;
                std::vector<uint32_t> meshIndices;
                std::unique_ptr<Skeleton> meshSkeleton;

                ProcessMesh(mesh, scene, meshVertices, meshIndices, meshSkeleton, false);

                std::string meshName = GenerateUniqueName(mesh->mName.C_Str());
                m_Meshes[meshName] = std::make_shared<Mesh>(meshVertices, meshIndices, std::move(meshSkeleton));
            }
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            ProcessOBJNode(node->mChildren[i], scene, spec, vertices, indices, skeleton);
        }
    }

    void MeshManager::ProcessFBXNode(aiNode* node, const aiScene* scene, const FBXSpec& spec,
        std::unordered_map<aiNode*, std::shared_ptr<Mesh>>& nodeMap) {

        if (node->mNumMeshes > 0) {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::unique_ptr<Skeleton> skeleton;

            for (uint32_t i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ProcessMesh(mesh, scene, vertices, indices, skeleton, spec.LoadSkeleton);
            }

            std::string meshName = GenerateUniqueName(node->mName.C_Str());
            auto newMesh = std::make_shared<Mesh>(vertices, indices, std::move(skeleton));
            m_Meshes[meshName] = newMesh;

            if (node->mParent) {
                auto parentIt = nodeMap.find(node->mParent);
                if (parentIt != nodeMap.end()) {
                    newMesh->m_Parent = parentIt->second;
                    parentIt->second->m_Children.push_back(newMesh);
                }
            }

            nodeMap[node] = newMesh;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            ProcessFBXNode(node->mChildren[i], scene, spec, nodeMap);
        }
    }

    void MeshManager::ProcessMesh(aiMesh* mesh, const aiScene* scene,
        std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
        std::unique_ptr<Skeleton>& skeleton, bool loadSkeleton) {

        uint32_t baseVertex = vertices.size();


        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;

            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;

            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f);
            }

            vertex.BoneIDs = glm::ivec4(0);
            vertex.Weights = glm::vec4(0.0f);

            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j] + baseVertex);
            }
        }

        if (loadSkeleton && mesh->HasBones()) {
            if (!skeleton) {
                skeleton = std::make_unique<Skeleton>();
            }

            for (uint32_t i = 0; i < mesh->mNumBones; i++) {
                aiBone* aiBone = mesh->mBones[i];
                std::string boneName = aiBone->mName.C_Str();

                auto& boneMap = skeleton->BoneNameToIndex;
                auto it = boneMap.find(boneName);
                if (it == boneMap.end()) {
                    Bone bone;
                    bone.Name = boneName;
                    bone.ParentIndex = -1;
                    bone.InverseBindMatrix = glm::transpose(glm::make_mat4(&aiBone->mOffsetMatrix.a1));

                    boneMap[boneName] = skeleton->Bones.size();
                    skeleton->Bones.push_back(bone);
                }

                int boneIndex = boneMap[boneName];

                for (uint32_t j = 0; j < aiBone->mNumWeights; j++) {
                    const aiVertexWeight& weight = aiBone->mWeights[j];
                    uint32_t vertexID = baseVertex + weight.mVertexId;

                    if (vertexID >= vertices.size()) continue;

                    Vertex& vertex = vertices[vertexID];
                    for (int k = 0; k < 4; k++) {
                        if (vertex.Weights[k] == 0.0f) {
                            vertex.BoneIDs[k] = boneIndex;
                            vertex.Weights[k] = weight.mWeight;
                            break;
                        }
                    }
                }
            }
        }
    }

    std::string MeshManager::GenerateUniqueName(const std::string& baseName) {
        std::string uniqueName = baseName;
        int counter = 1;

        if (m_Meshes.find(uniqueName) != m_Meshes.end()) {
            while (true) {
                std::string numberStr = (counter < 10) ? "0" + std::to_string(counter) : std::to_string(counter);
                uniqueName = baseName + numberStr;
                if (m_Meshes.find(uniqueName) == m_Meshes.end()) {
                    break;
                }
                counter++;
            }
        }

        return uniqueName;
    }

    bool MeshManager::UnloadMesh(const std::string& meshName) {
        auto it = m_Meshes.find(meshName);
        if (it != m_Meshes.end()) {
            m_Meshes.erase(it);
            return true;
        }
        return false;
    }

    bool MeshManager::RenameMesh(const std::string& oldName, const std::string& newName) {
        if (m_Meshes.find(newName) != m_Meshes.end() || newName.empty())
            return false;

        auto it = m_Meshes.find(oldName);
        if (it != m_Meshes.end()) {
            m_Meshes[newName] = it->second;
            m_Meshes.erase(it);
            return true;
        }
        return false;
    }

    std::shared_ptr<Mesh> MeshManager::GetMesh(const std::string& meshName) {
        auto it = m_Meshes.find(meshName);
        if (it != m_Meshes.end()) {
            return it->second;
        }
        return nullptr;
    }
}
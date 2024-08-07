#include "strikepch.h"
#include "ModelManager.h"
#include "StrikeEngine/Renderer/Core/Texture.h"
#include "StrikeEngine/Renderer/Core/Model.h"
#include <iostream>
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
        auto it = m_Models.find(path);
        return (it != m_Models.end()) ? it->second : nullptr;
    }

    void ModelManager::Clear() {
        for (auto& pair : m_Models) {
            delete pair.second;
        }
        m_Models.clear();
    }

    std::vector<ModelPart*> ModelManager::ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory) {
        std::vector<ModelPart*> parts;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ModelPart* part = ProcessMesh(mesh, scene, directory);
            parts.push_back(part);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            auto childParts = ProcessNode(node->mChildren[i], scene, directory);
            parts.insert(parts.end(), childParts.begin(), childParts.end());
        }

        return parts;
    }

    ModelPart* ModelManager::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        ExtractMeshData(mesh, vertices, indices);

        unsigned int vao, vbo, ebo;
        SetupMeshBuffers(vertices, indices, vao, vbo, ebo);

        ModelPart* modelPart = new ModelPart(vao, indices.size(), vbo, ebo);

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        Material mat = ExtractMaterial(material);

        modelPart->SetMaterial(mat);

        auto diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        modelPart->AddTextures(diffuseTextures);

        auto specularTextures = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        modelPart->AddTextures(specularTextures);

        auto normalTextures = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", directory);
        modelPart->AddTextures(normalTextures);

        auto heightTextures = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height", directory);
        modelPart->AddTextures(heightTextures);

        glm::vec3 min(std::numeric_limits<float>::max());
        glm::vec3 max(std::numeric_limits<float>::lowest());
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            glm::vec3 position(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
            min = glm::min(min, position);
            max = glm::max(max, position);
        }
        modelPart->SetAABB({ min, max });


        return modelPart;
    }


    void ModelManager::ExtractMeshData(aiMesh* mesh, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            if (mesh->HasNormals()) {
                vertices.push_back(mesh->mNormals[i].x);
                vertices.push_back(mesh->mNormals[i].y);
                vertices.push_back(mesh->mNormals[i].z);
            }
            else {
                vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
            }

            if (mesh->HasTextureCoords(0)) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertices.insert(vertices.end(), { 0.0f, 0.0f });
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
        }
    }

    void ModelManager::SetupMeshBuffers(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, unsigned int& vao, unsigned int& vbo, unsigned int& ebo) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    Material ModelManager::ExtractMaterial(aiMaterial* material) {
        Material mat = DEFAULT_MATERIAL;
        aiColor3D color;
        float shininess;

        if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            mat.ambient = glm::vec3(color.r, color.g, color.b);
        }

        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            mat.diffuse = glm::vec3(color.r, color.g, color.b);
        }

        if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            mat.specular = glm::vec3(color.r, color.g, color.b);
        }

        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            mat.shininess = shininess;
        }

        return mat;
    }

    std::vector<Texture*> ModelManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory) {
        std::vector<Texture*> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string fullPath = directory + '/' + std::string(str.C_Str());

            bool skip = false;
            for (auto& texture : textures) {
                if (texture->GetPath() == fullPath) {
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


    Model* ModelManager::CreateSphere(float radius, unsigned int rings, unsigned int sectors)
    {
        ModelPart* spherePart = CreateSphereMesh(radius, rings, sectors);
        Model* sphereModel = new Model();
        sphereModel->AddPart(spherePart);
        return sphereModel;
    }

    Model* ModelManager::CreateCuboid(float width, float height, float depth)
    {
        ModelPart* cuboidPart = CreateCuboidMesh(width, height, depth);
        Model* cuboidModel = new Model();
        cuboidModel->AddPart(cuboidPart);
        return cuboidModel;
    }

    Model* ModelManager::CreatePlane(float width, float height)
    {
        ModelPart* planePart = CreatePlaneMesh(width, height);
        Model* planeModel = new Model();
        planeModel->AddPart(planePart);
        return planeModel;
    }

    Model* ModelManager::CreateCylinder(float radius, float height, unsigned int sectors)
    {
        ModelPart* cylinderPart = CreateCylinderMesh(radius, height, sectors);
        Model* cylinderModel = new Model();
        cylinderModel->AddPart(cylinderPart);
        return cylinderModel;
    }

    ModelPart* ModelManager::CreateSphereMesh(float radius, unsigned int rings, unsigned int sectors)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float const R = 1.0f / (float)(rings - 1);
        float const S = 1.0f / (float)(sectors - 1);

        for (unsigned int r = 0; r < rings; ++r) {
            for (unsigned int s = 0; s < sectors; ++s) {
                float const y = sin(-glm::half_pi<float>() + glm::pi<float>() * r * R);
                float const x = cos(2 * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
                float const z = sin(2 * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);

                vertices.push_back(x * radius);
                vertices.push_back(y * radius);
                vertices.push_back(z * radius);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                vertices.push_back(s * S);
                vertices.push_back(r * R);
            }
        }

        for (unsigned int r = 0; r < rings - 1; ++r) {
            for (unsigned int s = 0; s < sectors - 1; ++s) {
                indices.push_back(r * sectors + s);
                indices.push_back(r * sectors + (s + 1));
                indices.push_back((r + 1) * sectors + (s + 1));

                indices.push_back(r * sectors + s);
                indices.push_back((r + 1) * sectors + (s + 1));
                indices.push_back((r + 1) * sectors + s);
            }
        }

        unsigned int vao, vbo, ebo;
        SetupMeshBuffers(vertices, indices, vao, vbo, ebo);

        return new ModelPart(vao, indices.size(), vbo, ebo);
    }

    ModelPart* ModelManager::CreateCuboidMesh(float width, float height, float depth)
    {
        std::vector<float> vertices = {
            // Front face
            -width / 2, -height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
             width / 2, -height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
             width / 2,  height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -width / 2,  height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
            // Back face
            -width / 2, -height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            -width / 2,  height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             width / 2,  height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
             width / 2, -height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             // Top face
             -width / 2,  height / 2, -depth / 2,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
             -width / 2,  height / 2,  depth / 2,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
              width / 2,  height / 2,  depth / 2,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
              width / 2,  height / 2, -depth / 2,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
              // Bottom face
              -width / 2, -height / 2, -depth / 2,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
               width / 2, -height / 2, -depth / 2,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
               width / 2, -height / 2,  depth / 2,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
              -width / 2, -height / 2,  depth / 2,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
              // Right face
               width / 2, -height / 2, -depth / 2,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
               width / 2,  height / 2, -depth / 2,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
               width / 2,  height / 2,  depth / 2,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
               width / 2, -height / 2,  depth / 2,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
               // Left face
               -width / 2, -height / 2, -depth / 2, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
               -width / 2, -height / 2,  depth / 2, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
               -width / 2,  height / 2,  depth / 2, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
               -width / 2,  height / 2, -depth / 2, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f
        };

        std::vector<unsigned int> indices = {
             0,  1,  2,  2,  3,  0,
             4,  5,  6,  6,  7,  4,
             8,  9, 10, 10, 11,  8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        unsigned int vao, vbo, ebo;
        SetupMeshBuffers(vertices, indices, vao, vbo, ebo);

        return new ModelPart(vao, indices.size(), vbo, ebo);
    }

    ModelPart* ModelManager::CreatePlaneMesh(float width, float height)
    {
        std::vector<float> vertices = {
            -width / 2, 0.0f, -height / 2,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
             width / 2, 0.0f, -height / 2,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             width / 2, 0.0f,  height / 2,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            -width / 2, 0.0f,  height / 2,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vao, vbo, ebo;
        SetupMeshBuffers(vertices, indices, vao, vbo, ebo);

        return new ModelPart(vao, indices.size(), vbo, ebo);
    }

    ModelPart* ModelManager::CreateCylinderMesh(float radius, float height, unsigned int sectors)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float const S = 1.0f / static_cast<float>(sectors);

        // Vertices for the sides
        for (unsigned int i = 0; i <= sectors; ++i) {
            float const x = cos(2 * glm::pi<float>() * i * S);
            float const z = sin(2 * glm::pi<float>() * i * S);

            // Bottom vertex
            vertices.push_back(x * radius);
            vertices.push_back(-height / 2);
            vertices.push_back(z * radius);
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(z);
            vertices.push_back(i * S);
            vertices.push_back(0.0f);

            // Top vertex
            vertices.push_back(x * radius);
            vertices.push_back(height / 2);
            vertices.push_back(z * radius);
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(z);
            vertices.push_back(i * S);
            vertices.push_back(1.0f);
        }

        // Indices for the sides
        for (unsigned int i = 0; i < sectors; ++i) {
            indices.push_back(2 * i);
            indices.push_back(2 * i + 1);
            indices.push_back(2 * i + 2);

            indices.push_back(2 * i + 2);
            indices.push_back(2 * i + 1);
            indices.push_back(2 * i + 3);
        }

        // Add center vertices for top and bottom caps
        vertices.push_back(0.0f);
        vertices.push_back(-height / 2);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.5f);

        vertices.push_back(0.0f);
        vertices.push_back(height / 2);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.5f);

        unsigned int bottomCenterIndex = vertices.size() / 8 - 2;
        unsigned int topCenterIndex = vertices.size() / 8 - 1;

        // Indices for bottom cap
        for (unsigned int i = 0; i < sectors; ++i) {
            indices.push_back(bottomCenterIndex);
            indices.push_back(2 * i);
            indices.push_back(2 * (i + 1));
        }

        // Indices for top cap
        for (unsigned int i = 0; i < sectors; ++i) {
            indices.push_back(topCenterIndex);
            indices.push_back(2 * (i + 1) + 1);
            indices.push_back(2 * i + 1);
        }

        unsigned int vao, vbo, ebo;
        SetupMeshBuffers(vertices, indices, vao, vbo, ebo);

        return new ModelPart(vao, indices.size(), vbo, ebo);
    }

}

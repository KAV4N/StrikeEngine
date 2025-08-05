#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <pugixml.hpp>

#include "StrikeEngine/Asset/Types/Mesh.h"
#include "StrikeEngine/Asset/Types/Material.h"

namespace StrikeEngine {

    struct EntityData {
        std::string name;
        std::string id;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        std::string meshId;
        std::vector<std::string> materialIds;
        std::vector<std::shared_ptr<EntityData>> children;
    };

    class ModelParser {
    private:
        Assimp::Importer mImporter;
        std::vector<std::shared_ptr<Mesh>> mMeshes;
        std::vector<std::shared_ptr<Material>> mMaterials;
        std::shared_ptr<EntityData> mRootEntity;
        std::string mIdPrefix;

        void processMaterials(const aiScene* scene);
        void processMeshes(const aiScene* scene);
        void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent = nullptr);
        glm::vec3 aiVector3dToGlm(const aiVector3D& vec);
        glm::vec3 aiColor3dToGlm(const aiColor3D& color);
        glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& mat);
        void decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
        void saveMeshToXml(const Mesh& mesh, const std::string& templateDir);
        void saveMaterialToXml(const Material& material, const std::string& templateDir);
        void saveTemplateXml(const std::string& templateName, const std::string& sourceFile, const std::string& templateDir);
        void writeEntityToXml(pugi::xml_node& parent, const std::shared_ptr<EntityData>& entity);
        std::string sanitizeId(const std::string& name);
        void calculateMeshBounds(Mesh& mesh);
        void calculateSubMeshBounds(const Mesh& mesh, SubMeshData& subMeshData);
        void combineMeshes(const std::vector<unsigned int>& meshIndices, const aiScene* scene, Mesh& combinedMesh);
        void reset();

    public:
        ModelParser();
        ~ModelParser();
        bool parseModel(const std::string& modelPath, const std::string& templateDir);
    };
}
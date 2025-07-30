#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>
// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// PugiXML includes
#include <pugixml.hpp>

#include "StrikeEngine/Graphics/Core/Assets/Mesh.h"
#include "StrikeEngine/Graphics/Core/Assets/Material.h"

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
        Assimp::Importer importer;
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Material>> materials;
        std::shared_ptr<EntityData> rootEntity;
        std::string idPrefix;
        void processMaterials(const aiScene* scene);
        void processMeshes(const aiScene* scene);
        void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent = nullptr);
        glm::vec3 aiVector3DToGlm(const aiVector3D& vec);
        glm::vec3 aiColor3DToGlm(const aiColor3D& color);
        glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& mat);
        void decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
        void saveMeshToXML(const Mesh& mesh, const std::string& templateDir);
        void saveMaterialToXML(const Material& material, const std::string& templateDir);
        void saveTemplateXML(const std::string& templateName, const std::string& sourceFile, const std::string& templateDir);
        void writeEntityToXML(pugi::xml_node& parent, const std::shared_ptr<EntityData>& entity);
        std::string sanitizeId(const std::string& name);
        void calculateMeshBounds(Mesh& mesh);
        void calculateSubMeshBounds(const Mesh& mesh, SubMeshData& SubMeshData);
        void combineMeshes(const std::vector<unsigned int>& meshIndices, const aiScene* scene, Mesh& combinedMesh);
        void reset();
    public:
        ModelParser();
        ~ModelParser();
        bool parseModel(const std::string& modelPath, const std::string& templateDir);
    };
}
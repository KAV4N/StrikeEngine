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
#include <glm/gtx/quaternion.hpp>

#include <pugixml.hpp>

namespace StrikeEngine {

    class Model;
    class Material;

    struct EntityData {
        std::string tag;

        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 0.0f };
        glm::vec3 scale{ 1.0f };

        std::string modelId;
        int32_t meshIdx = -1;  // -1 = no mesh/renderer
        std::string materialId;

        std::vector<std::shared_ptr<EntityData>> children;
    };

    class ModelParser {
    public:
        ModelParser();
        ~ModelParser();

        // Main function: parse model and generate template + material files
        bool parseModel(const std::filesystem::path& modelPath);

    private:
        // Process Assimp scene
        void processScene(const aiScene* scene, const std::filesystem::path& modelDir);
        void processMaterials(const aiScene* scene, const std::filesystem::path& modelDir);
        void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent = nullptr);

        // Texture processing
        std::string processTexture(aiMaterial* aiMat, aiTextureType type, const std::string& typeName, unsigned int materialIndex);

        // Helpers
        glm::mat4 aiMatrixToGlm(const aiMatrix4x4& from);
        void decomposeTransform(const glm::mat4& transform, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale);

        // Output
        void saveMaterialToFile(const std::shared_ptr<Material>& material, const std::filesystem::path& matPath);
        void saveTemplateXml(const std::filesystem::path& templatePath);

        void writeEntityToXml(pugi::xml_node& parentNode, const std::shared_ptr<EntityData>& entity);

    private:
        Assimp::Importer mImporter;

        std::shared_ptr<Model> mModel;
        std::vector<std::shared_ptr<Material>> mMaterials;
        std::vector<std::shared_ptr<EntityData>> mRootEntities;

        std::filesystem::path mModelDirectory;
        std::string mModelName;
        std::string mModelId;
    };

} // namespace StrikeEngine
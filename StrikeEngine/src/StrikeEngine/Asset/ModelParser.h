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
    struct EntityData {
        std::string tag;

        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 0.0f };
        glm::vec3 scale{ 1.0f };

        std::string modelId;
        int32_t meshIdx = -1;  // -1 = no mesh/renderer
        
        glm::uvec3 color{ 255u, 255u, 255u }; // RGB (0–255 integers)

        std::vector<std::shared_ptr<EntityData>> children;
    };

    class ModelParser {
    public:
        ModelParser();
        ~ModelParser();

        /**
         * @brief Parse a 3D model file and convert it into the engine's Model format.
         * @param modelPath The file path to the 3D model.
         * @return True if parsing was successful, false otherwise.
         */
        bool parseModel(const std::filesystem::path& modelPath);

    private:
        // Process Assimp scene
        void processScene(const aiScene* scene, const std::filesystem::path& modelDir);
        void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent = nullptr);

        // Helpers
        glm::mat4 aiMatrixToGlm(const aiMatrix4x4& from);
        void decomposeTransform(const glm::mat4& transform, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale);
        glm::uvec3 extractColor(const aiMaterial* material);

        // Output
        void saveTemplateXml(const std::filesystem::path& templatePath);
        void writeEntityToXml(pugi::xml_node& parentNode, const std::shared_ptr<EntityData>& entity);

    private:
        Assimp::Importer mImporter;

        std::shared_ptr<Model> mModel;
        std::vector<std::shared_ptr<EntityData>> mRootEntities;

        std::filesystem::path mModelDirectory;
        std::string mModelName;
        std::string mModelId;
    };

} // namespace StrikeEngine
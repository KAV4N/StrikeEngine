#include "strikepch.h"
#include "ModelParser.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    ModelParser::ModelParser() {}

    ModelParser::~ModelParser() {}

    bool ModelParser::parseModel(const std::filesystem::path& modelPath) {
        if (modelPath.extension() == ".tmpl") {
            return true;
        }

        if (std::filesystem::exists(modelPath)) {
            std::filesystem::path modelDir = modelPath.parent_path();
            std::filesystem::path modelStem = modelPath.stem();
            std::filesystem::path templatePath = modelDir / (modelStem.string() + ".tmpl");
            
            if (std::filesystem::exists(templatePath)) {
                return true;
            }
        }

        if (!std::filesystem::exists(modelPath)) {
            std::cerr << "Model file not found: " << modelPath << std::endl;
            return false;
        }

        mModelDirectory = modelPath.parent_path();
        mModelName = modelPath.stem().string();
        mModelId = mModelName; 

        const aiScene* scene = mImporter.ReadFile(modelPath.string(),
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << mImporter.GetErrorString() << std::endl;
            return false;
        }

        mModel = std::make_shared<Model>(mModelId, modelPath);
        mMaterials.clear();
        mRootEntities.clear();

        processMaterials(scene, mModelDirectory);
        processScene(scene, mModelDirectory);

        std::filesystem::path templatePath = mModelDirectory / (mModelName + ".tmpl");
        
        saveTemplateXml(templatePath);

        return true;
    }

    void ModelParser::processScene(const aiScene* scene, const std::filesystem::path& modelDir) {
        aiNode* root = scene->mRootNode;
        
        if (root->mNumChildren > 0) {
            for (unsigned int i = 0; i < root->mNumChildren; ++i) {
                processNode(root->mChildren[i], scene, nullptr);
            }
        } else if (root->mNumMeshes > 0) {
            processNode(root, scene, nullptr);
        }
    }

    void ModelParser::processMaterials(const aiScene* scene, const std::filesystem::path& modelDir) {
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* aiMat = scene->mMaterials[i];
            aiString matName;
            aiMat->Get(AI_MATKEY_NAME, matName);

            std::string materialName = matName.C_Str();
            if (materialName.empty()) {
                materialName = "Material_" + std::to_string(i);
            }
            
            std::string materialId = mModelName + "_" + materialName;

            auto material = std::make_shared<Material>(materialId, materialId + ".mat");

            aiColor3D baseColor(1.0f, 1.0f, 1.0f);
            float metallic = 0.0f;
            float roughness = 1.0f;

            aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
            aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
            aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

            material->setBaseColor(glm::ivec3(
                static_cast<int>(baseColor.r * 255),
                static_cast<int>(baseColor.g * 255),
                static_cast<int>(baseColor.b * 255)
            ));
            material->setMetallic(metallic);
            material->setRoughness(roughness);

            mMaterials.push_back(material);

            processTexture(aiMat, aiTextureType_DIFFUSE, "BaseColor", i);
            processTexture(aiMat, aiTextureType_METALNESS, "Metallic", i);
            processTexture(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, "Roughness", i);
        }
    }

    std::string ModelParser::processTexture(aiMaterial* aiMat, aiTextureType type, 
                                           const std::string& typeName, unsigned int materialIndex) {
        if (aiMat->GetTextureCount(type) == 0) {
            return "";
        }

        aiString texPath;
        if (aiMat->GetTexture(type, 0, &texPath) != AI_SUCCESS) {
            return "";
        }

        std::string texPathStr = texPath.C_Str();
        if (texPathStr.empty()) {
            return "";
        }

        std::string texFilename = texPathStr;
        size_t lastSlash = texFilename.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            texFilename = texFilename.substr(lastSlash + 1);
        }

        std::string textureId = mModelName + "_" + typeName + "_" + std::to_string(materialIndex);

        std::filesystem::path fullTexturePath = "@" / mModelDirectory / texFilename;

        auto texture = std::make_shared<Texture>(textureId, fullTexturePath);

        auto& material = mMaterials[materialIndex];

        if (typeName == "BaseColor") {
            material->mTextures[static_cast<uint32_t>(TextureSlot::BaseColor)] = texture;
        } else if (typeName == "Metallic") {
            material->mTextures[static_cast<uint32_t>(TextureSlot::Metallic)] = texture;
        } else if (typeName == "Roughness") {
            material->mTextures[static_cast<uint32_t>(TextureSlot::Roughness)] = texture;
        }

        return textureId;
    }

    void ModelParser::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent) {
        glm::mat4 localTransform = aiMatrixToGlm(node->mTransformation);

        auto entity = std::make_shared<EntityData>();
        entity->tag = node->mName.C_Str();
        if (entity->tag.empty()) entity->tag = "Node";

        decomposeTransform(localTransform, entity->position, entity->rotation, entity->scale);

        if (node->mNumMeshes > 0) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
            
            if (mModel) {
                auto meshPtr = std::make_shared<Mesh>();
                mModel->addMesh(meshPtr);

                entity->modelId = mModelId;
                entity->meshIdx = static_cast<int32_t>(mModel->getMeshCount() - 1);

                unsigned int matIndex = mesh->mMaterialIndex;
                if (matIndex < mMaterials.size()) {
                    entity->materialId = mMaterials[matIndex]->getId();
                }
            }
        }

        if (parent) {
            parent->children.push_back(entity);
        } else {
            mRootEntities.push_back(entity);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene, entity);
        }
    }

    glm::mat4 ModelParser::aiMatrixToGlm(const aiMatrix4x4& from) {
        glm::mat4 to;
        to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
        to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
        to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
        to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;
        return to;
    }

    void ModelParser::decomposeTransform(const glm::mat4& transform, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale) {
        pos = glm::vec3(transform[3]);

        scale.x = glm::length(glm::vec3(transform[0]));
        scale.y = glm::length(glm::vec3(transform[1]));
        scale.z = glm::length(glm::vec3(transform[2]));

        if (scale.x < 0.0001f) scale.x = 1.0f;
        if (scale.y < 0.0001f) scale.y = 1.0f;
        if (scale.z < 0.0001f) scale.z = 1.0f;

        glm::mat3 rotMat(
            glm::vec3(transform[0]) / scale.x,
            glm::vec3(transform[1]) / scale.y,
            glm::vec3(transform[2]) / scale.z
        );
        
        glm::quat quat = glm::quat_cast(rotMat);
        rot = glm::degrees(glm::eulerAngles(quat));
    }

    void ModelParser::saveMaterialToFile(const std::shared_ptr<Material>& material, const std::filesystem::path& matPath) {
        material->serialize(matPath);
    }

    void ModelParser::saveTemplateXml(const std::filesystem::path& templatePath) {
        pugi::xml_document doc;
        auto decl = doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "UTF-8";

        auto templateNode = doc.append_child("template");
        templateNode.append_attribute("name") = mModelName.c_str();
        templateNode.append_attribute("source") = mModel->getPath().filename().string().c_str();

        auto assets = templateNode.append_child("assets");

        auto modelAsset = assets.append_child("model");
        modelAsset.append_attribute("id") = mModelId.c_str();
        modelAsset.append_attribute("src") = mModel->getPath().filename().string().c_str();

        for (const auto& mat : mMaterials) {
            auto matAsset = assets.append_child("material");
            matAsset.append_attribute("id") = mat->getId().c_str();
            
            std::filesystem::path matFilePath = mat->getId() + ".mat";
            matAsset.append_attribute("src") = matFilePath.string().c_str();

            std::filesystem::path fullMatPath = mModelDirectory / matFilePath;
            saveMaterialToFile(mat, fullMatPath);
        }

        auto entities = templateNode.append_child("entities");
        for (const auto& rootEntity : mRootEntities) {
            writeEntityToXml(entities, rootEntity);
        }

        doc.save_file(templatePath.string().c_str(), "  ");
    }

    void ModelParser::writeEntityToXml(pugi::xml_node& parentNode, const std::shared_ptr<EntityData>& entity) {
        auto entityNode = parentNode.append_child("entity");
        entityNode.append_attribute("tag") = entity->tag.c_str();

        auto formatFloat = [](float value) -> std::string {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << value;
            return oss.str();
        };

        auto posStr = formatFloat(entity->position.x) + "," + formatFloat(entity->position.y) + "," + formatFloat(entity->position.z);
        auto rotStr = formatFloat(entity->rotation.x) + "," + formatFloat(entity->rotation.y) + "," + formatFloat(entity->rotation.z);
        auto scaleStr = formatFloat(entity->scale.x) + "," + formatFloat(entity->scale.y) + "," + formatFloat(entity->scale.z);

        entityNode.append_attribute("position") = posStr.c_str();
        entityNode.append_attribute("rotation") = rotStr.c_str();
        entityNode.append_attribute("scale") = scaleStr.c_str();

        if (entity->meshIdx >= 0 && !entity->modelId.empty()) {
            auto components = entityNode.append_child("components");
            auto renderer = components.append_child("renderer");
            renderer.append_attribute("model") = entity->modelId.c_str();
            renderer.append_attribute("mesh") = entity->meshIdx;
            if (!entity->materialId.empty()) {
                renderer.append_attribute("material") = entity->materialId.c_str();
            }
        }

        for (const auto& child : entity->children) {
            writeEntityToXml(entityNode, child);
        }
    }

} // namespace StrikeEngine
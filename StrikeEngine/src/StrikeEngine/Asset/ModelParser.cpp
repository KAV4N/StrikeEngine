#include "strikepch.h"
#include "ModelParser.h"
#include "StrikeEngine/Asset/Types/Model.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace Strike {

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
            STRIKE_CORE_ERROR("Model file does not exist: {}", modelPath.string());
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
            STRIKE_CORE_ERROR("Assimp error loading model '{}': {}", 
                modelPath.string(), mImporter.GetErrorString());
            return false;
        }

        mModel = std::make_shared<Model>(mModelId, modelPath);
        mRootEntities.clear();
        mMeshCount = 0;

        processScene(scene, mModelDirectory);

        std::filesystem::path templatePath = mModelDirectory / (mModelName + ".tmpl");
        saveTemplateXml(templatePath);

        return true;
    }

    void ModelParser::processScene(const aiScene* scene, const std::filesystem::path& modelDir) {
        processNode(scene->mRootNode, scene, nullptr);
    }

    void ModelParser::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent) {
        glm::mat4 localTransform = aiMatrixToGlm(node->mTransformation);

        auto entity = std::make_shared<EntityData>();
        entity->tag = node->mName.C_Str();
        if (entity->tag.empty()) entity->tag = "Node";

        decomposeTransform(localTransform, entity->position, entity->rotation, entity->scale);

        for (unsigned int m = 0; m < node->mNumMeshes; ++m) {
            unsigned int meshIndex = node->mMeshes[m];
            aiMesh* mesh = scene->mMeshes[meshIndex];

            if (mModel) {
                std::shared_ptr<EntityData> meshOwner = entity;
                if (node->mNumMeshes > 1) {
                    meshOwner = std::make_shared<EntityData>();
                    meshOwner->tag = entity->tag + "_mesh" + std::to_string(m);
                    meshOwner->position = glm::vec3(0.0f);
                    meshOwner->rotation = glm::vec3(0.0f);
                    meshOwner->scale    = glm::vec3(1.0f);
                    entity->children.push_back(meshOwner);
                }

                auto meshPtr = std::make_shared<Mesh>();
                mModel->addMesh(meshPtr);
                meshOwner->modelId  = mModelId;
                meshOwner->meshIdx  = static_cast<int32_t>(mMeshCount++);

                if (mesh->mMaterialIndex < scene->mNumMaterials) {
                    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                    meshOwner->color = extractColor(material);
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

    glm::uvec3 ModelParser::extractColor(const aiMaterial* material) {
        aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);

        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != AI_SUCCESS) {
            material->Get(AI_MATKEY_BASE_COLOR, diffuseColor);
        }

        return glm::uvec3(
            static_cast<unsigned int>(glm::clamp(diffuseColor.r * 255.0f + 0.5f, 0.0f, 255.0f)),
            static_cast<unsigned int>(glm::clamp(diffuseColor.g * 255.0f + 0.5f, 0.0f, 255.0f)),
            static_cast<unsigned int>(glm::clamp(diffuseColor.b * 255.0f + 0.5f, 0.0f, 255.0f))
        );
    }

    void ModelParser::saveTemplateXml(const std::filesystem::path& templatePath) {
        pugi::xml_document doc;
        auto decl = doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version")  = "1.0";
        decl.append_attribute("encoding") = "UTF-8";

        auto templateNode = doc.append_child("template");

        auto assets    = templateNode.append_child("assets");
        auto modelAsset = assets.append_child("model");
        modelAsset.append_attribute("id")  = mModelId.c_str();
        modelAsset.append_attribute("src") = mModel->getPath().filename().string().c_str();

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

        auto posStr   = formatFloat(entity->position.x) + "," + formatFloat(entity->position.y) + "," + formatFloat(entity->position.z);
        auto rotStr   = formatFloat(entity->rotation.x) + "," + formatFloat(entity->rotation.y) + "," + formatFloat(entity->rotation.z);
        auto scaleStr = formatFloat(entity->scale.x)    + "," + formatFloat(entity->scale.y)    + "," + formatFloat(entity->scale.z);

        entityNode.append_attribute("position") = posStr.c_str();
        entityNode.append_attribute("rotation") = rotStr.c_str();
        entityNode.append_attribute("scale")    = scaleStr.c_str();

        if (entity->meshIdx >= 0 && !entity->modelId.empty()) {
            auto components = entityNode.append_child("components");
            auto renderer   = components.append_child("renderer");
            renderer.append_attribute("model") = entity->modelId.c_str();
            renderer.append_attribute("mesh")  = entity->meshIdx;

            char colorStr[32];
            snprintf(colorStr, sizeof(colorStr), "%u,%u,%u",
                     entity->color.r, entity->color.g, entity->color.b);
            renderer.append_attribute("color") = colorStr;
        }

        for (const auto& child : entity->children) {
            writeEntityToXml(entityNode, child);
        }
    }

}
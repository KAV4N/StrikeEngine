#include "ModelParser.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace StrikeEngine {

    ModelParser::ModelParser() {}

    ModelParser::~ModelParser() {}

    bool ModelParser::parseModel(const std::filesystem::path& modelPath) {
        if (modelPath.extension() == ".tmpl") {
            return true;
        }

        // Check if modelPath exists and if a .tmpl file already exists in the same directory
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
            aiProcess_CalcTangentSpace |
            aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << mImporter.GetErrorString() << std::endl;
            return false;
        }

        // Reset state
        mModel = std::make_shared<Model>(mModelId, modelPath);
        mMaterials.clear();
        mRootEntities.clear();

        processMaterials(scene, mModelDirectory);
        processScene(scene, mModelDirectory);

        // Generate output file paths in the same directory as the model
        std::filesystem::path templatePath = mModelDirectory / (mModelName + ".tmpl");
        
        // Save everything
        saveTemplateXml(templatePath);

        return true;
    }

    void ModelParser::processScene(const aiScene* scene, const std::filesystem::path& modelDir) {
        processNode(scene->mRootNode, scene);
        if (mModel) {
            mModel->postLoad(); // Setup GPU resources
        }
    }

    void ModelParser::processMaterials(const aiScene* scene, const std::filesystem::path& modelDir) {
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* aiMat = scene->mMaterials[i];
            aiString matName;
            aiMat->Get(AI_MATKEY_NAME, matName);

            // Use actual material name from the model, sanitized for filename use
            std::string materialName = matName.C_Str();
            if (materialName.empty()) {
                materialName = "Material_" + std::to_string(i);
            }
            
            // Create material ID using model name as prefix and actual material name
            std::string materialId = mModelName + "_" + materialName;

            // Material files will be saved in the same directory as the model - use just filename
            auto material = std::make_shared<Material>(materialId, materialId + ".mat");

            // Default PBR values
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
        }
    }

    void ModelParser::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent, const glm::mat4& parentTransform) {
        glm::mat4 localTransform = aiMatrixToGlm(node->mTransformation);
        glm::mat4 globalTransform = parentTransform * localTransform;

        auto entity = std::make_shared<EntityData>();
        entity->tag = node->mName.C_Str();
        if (entity->tag.empty()) entity->tag = "Node";

        decomposeTransform(globalTransform, entity->position, entity->rotation, entity->scale);

        // Process meshes
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            
            if (mModel) {
                auto meshPtr = std::make_shared<Mesh>();

                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

                Bounds bounds;
                for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
                    Vertex vertex{};
                    vertex.position = glm::vec3(
                        mesh->mVertices[v].x,
                        mesh->mVertices[v].y,
                        mesh->mVertices[v].z
                    );
                    bounds.unionBounds(Bounds(vertex.position));

                    if (mesh->HasNormals()) {
                        vertex.normal = glm::vec3(
                            mesh->mNormals[v].x,
                            mesh->mNormals[v].y,
                            mesh->mNormals[v].z
                        );
                    }

                    if (mesh->HasTextureCoords(0)) {
                        vertex.texCoord = glm::vec2(
                            mesh->mTextureCoords[0][v].x,
                            mesh->mTextureCoords[0][v].y
                        );
                    }

                    if (mesh->HasTangentsAndBitangents()) {
                        vertex.tangent = glm::vec3(
                            mesh->mTangents[v].x,
                            mesh->mTangents[v].y,
                            mesh->mTangents[v].z
                        );
                    }

                    vertices.push_back(vertex);
                }

                for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
                    aiFace face = mesh->mFaces[f];
                    for (unsigned int idx = 0; idx < face.mNumIndices; ++idx) {
                        indices.push_back(face.mIndices[idx]);
                    }
                }

                meshPtr->setVertices(vertices);
                meshPtr->setIndices(indices);
                meshPtr->setBounds(bounds);

                mModel->addMesh(meshPtr);

                // Create renderer component data
                auto rendererEntity = std::make_shared<EntityData>();
                rendererEntity->tag = entity->tag;
                rendererEntity->position = glm::vec3(0);
                rendererEntity->rotation = glm::vec3(0);
                rendererEntity->scale = glm::vec3(1);

                rendererEntity->modelId = mModelId;
                rendererEntity->meshIdx = static_cast<uint32_t>(mModel->getMeshCount() - 1);

                unsigned int matIndex = mesh->mMaterialIndex;
                if (matIndex < mMaterials.size()) {
                    rendererEntity->materialId = mMaterials[matIndex]->getId();
                }

                entity->children.push_back(rendererEntity);
            }
        }

        // Add to parent or root
        if (parent) {
            parent->children.push_back(entity);
        } else {
            mRootEntities.push_back(entity);
        }

        // Recurse children
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene, entity, globalTransform);
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
        // Position
        pos = glm::vec3(transform[3]);

        // Scale
        scale.x = glm::length(glm::vec3(transform[0]));
        scale.y = glm::length(glm::vec3(transform[1]));
        scale.z = glm::length(glm::vec3(transform[2]));

        // Rotation (convert to Euler angles - simplified)
        glm::mat3 rotMat(
            transform[0] / scale.x,
            transform[1] / scale.y,
            transform[2] / scale.z
        );
        rot = glm::eulerAngles(glm::quat_cast(rotMat));
        rot = glm::degrees(rot);
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

        // Source path (use just the filename since it's in the same directory)
        templateNode.append_attribute("source") = mModel->getPath().filename().string().c_str();

        // <assets>
        auto assets = templateNode.append_child("assets");

        // Model asset - use just filename for src
        auto modelAsset = assets.append_child("model");
        modelAsset.append_attribute("id") = mModelId.c_str();
        modelAsset.append_attribute("src") = mModel->getPath().filename().string().c_str();

        // Material assets - use just filenames for src
        for (const auto& mat : mMaterials) {
            auto matAsset = assets.append_child("material");
            matAsset.append_attribute("id") = mat->getId().c_str();
             
            // Material files are in the same directory as the model - use just filename
            std::filesystem::path matFilePath = mat->getId() + ".mat";
            matAsset.append_attribute("src") = matFilePath.string().c_str();

            // Save material file to the same directory
            std::filesystem::path fullMatPath = mModelDirectory / matFilePath;
            saveMaterialToFile(mat, fullMatPath);
        }

        // <entities>
        auto entities = templateNode.append_child("entities");
        for (const auto& rootEntity : mRootEntities) {
            writeEntityToXml(entities, rootEntity);
        }

        doc.save_file(templatePath.string().c_str(), "  ");
    }

    void ModelParser::writeEntityToXml(pugi::xml_node& parentNode, const std::shared_ptr<EntityData>& entity) {
        auto entityNode = parentNode.append_child("entity");
        entityNode.append_attribute("tag") = entity->tag.c_str();

        // Helper function to format float values with fixed precision
        auto formatFloat = [](float value) -> std::string {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << value;
            return oss.str();
        };

        // Transform with formatted values
        auto posStr = formatFloat(entity->position.x) + "," + formatFloat(entity->position.y) + "," + formatFloat(entity->position.z);
        auto rotStr = formatFloat(entity->rotation.x) + "," + formatFloat(entity->rotation.y) + "," + formatFloat(entity->rotation.z);
        auto scaleStr = formatFloat(entity->scale.x) + "," + formatFloat(entity->scale.y) + "," + formatFloat(entity->scale.z);

        entityNode.append_attribute("position") = posStr.c_str();
        entityNode.append_attribute("rotation") = rotStr.c_str();
        entityNode.append_attribute("scale") = scaleStr.c_str();

        // Children
        for (const auto& child : entity->children) {
            if (!child->modelId.empty()) {
                // This is a renderer entity
                auto rendererEntity = entityNode.append_child("entity");
                rendererEntity.append_attribute("tag") = child->tag.c_str();
                rendererEntity.append_attribute("position") = "0.000,0.000,0.000";
                rendererEntity.append_attribute("rotation") = "0.000,0.000,0.000";
                rendererEntity.append_attribute("scale") = "1.000,1.000,1.000";

                auto components = rendererEntity.append_child("components");
                auto renderer = components.append_child("renderer");
                renderer.append_attribute("model") = child->modelId.c_str();
                renderer.append_attribute("meshIdx") = child->meshIdx;
                if (!child->materialId.empty()) {
                    renderer.append_attribute("material") = child->materialId.c_str();
                }
            } else {
                writeEntityToXml(entityNode, child);
            }
        }
    }

} // namespace StrikeEngine
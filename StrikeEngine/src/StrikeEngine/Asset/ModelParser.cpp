#include "ModelParser.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>

namespace StrikeEngine {

    ModelParser::ModelParser() {
        mImporter.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
            aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    }

    ModelParser::~ModelParser() {
        reset();
    }

    std::string ModelParser::generateUniqueId(const std::string& baseId) {
        if (mUsedIds.find(baseId) == mUsedIds.end()) {
            mUsedIds.insert(baseId);
            return baseId;
        }

        int counter = 1;
        std::string uniqueId;

        do {
            std::ostringstream oss;
            oss << baseId << "_" << counter;
            uniqueId = oss.str();
            counter++;
        } while (mUsedIds.find(uniqueId) != mUsedIds.end());

        mUsedIds.insert(uniqueId);
        return uniqueId;
    }


    bool ModelParser::parseModel(const std::filesystem::path& modelPath, const std::filesystem::path& templateSrc) {
        reset();

        if (!std::filesystem::exists(modelPath)) {
            std::cerr << "Model file does not exist: " << modelPath << std::endl;
            return false;
        }

        std::string filename = modelPath.stem().string();
        mIdPrefix = filename + ".";

        std::filesystem::path templateDir = templateSrc.parent_path();
        if (!std::filesystem::exists(templateDir)) {
            std::filesystem::create_directories(templateDir);
        }

        const aiScene* scene = mImporter.ReadFile(modelPath.string(),
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_LimitBoneWeights |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_SplitLargeMeshes |
            aiProcess_ValidateDataStructure |
            aiProcess_OptimizeMeshes);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Error loading model: " << mImporter.GetErrorString() << std::endl;
            reset();
            return false;
        }

        processMaterials(scene);
        processMeshes(scene);

        for (uint32_t i = 0; i < scene->mRootNode->mNumChildren; i++) {
            auto entity = std::make_shared<EntityData>();
            entity->name = scene->mRootNode->mChildren[i]->mName.C_Str();
            entity->id = generateUniqueId(entity->name);

            glm::mat4 transform = aiMatrix4x4ToGlm(scene->mRootNode->mChildren[i]->mTransformation);
            decomposeTransform(transform, entity->position, entity->rotation, entity->scale);
            mTopLevelEntities.push_back(entity);
            processNode(scene->mRootNode->mChildren[i], scene, entity);
        }

        for (const auto& mesh : mMeshes) {
            saveMeshToXml(*mesh, templateDir);
        }

        for (const auto& material : mMaterials) {
            saveMaterialToXml(*material, templateDir);
        }

        std::string templateName = modelPath.stem().string();
        saveTemplateXml(templateName, modelPath, templateSrc);

        reset();
        return true;
    }

    void ModelParser::processMaterials(const aiScene* scene) {
        mMaterials.clear();
        mMaterials.reserve(scene->mNumMaterials);

        for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* aiMat = scene->mMaterials[i];
            aiString name;
            aiMat->Get(AI_MATKEY_NAME, name);
            std::string matName = name.C_Str();

            std::string materialId = generateUniqueId(matName);

            // Create a standard Material instead of PBRMaterial
            auto material = std::make_shared<Material>(mIdPrefix + materialId, "", matName);

            // Set base color (diffuse) as a vec3 uniform
            aiColor3D baseColor(1.0f, 1.0f, 1.0f);
            if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
                material->setVec3("uBaseColor", aiColor3dToGlm(baseColor));
            }

            // Set metallic as a float uniform
            float metallic = 0.0f;
            if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
                material->setFloat("uMetallic", metallic);
            }
            else {
                aiColor3D specular;
                if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
                    float avgSpecular = (specular.r + specular.g + specular.b) / 3.0f;
                    material->setFloat("uMetallic", avgSpecular > 0.9f ? 1.0f : 0.0f);
                }
            }

            // Set roughness as a float uniform
            float roughness = 0.5f;
            if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
                material->setFloat("uRoughness", roughness);
            }
            else {
                float shininess = 0.0f;
                if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
                    roughness = std::sqrt(2.0f / (shininess + 2.0f));
                    material->setFloat("uRoughness", glm::clamp(roughness, 0.04f, 1.0f));
                }
            }

            // Set emission as a vec3 uniform
            aiColor3D emission(0.0f, 0.0f, 0.0f);
            if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emission) == AI_SUCCESS) {
                material->setVec3("uEmission", aiColor3dToGlm(emission));
            }

            // Set opacity as a float uniform
            float opacity = 1.0f;
            if (aiMat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
                material->setFloat("uOpacity", opacity);
            }

            mMaterials.push_back(material);
        }
    }

    void ModelParser::processMeshes(const aiScene* scene) {
        mMeshes.clear();
        std::unordered_map<aiNode*, std::vector<uint32_t>> nodeMeshes;

        std::function<void(aiNode*)> collectMeshes = [&](aiNode* node) {
            if (node->mNumMeshes > 0) {
                for (uint32_t i = 0; i < node->mNumMeshes; i++) {
                    nodeMeshes[node].push_back(node->mMeshes[i]);
                }
            }
            for (uint32_t i = 0; i < node->mNumChildren; i++) {
                collectMeshes(node->mChildren[i]);
            }
            };

        collectMeshes(scene->mRootNode);

        for (const auto& [node, meshIndices] : nodeMeshes) {
            for (uint32_t meshIndex : meshIndices) {
                aiMesh* aiMesh = scene->mMeshes[meshIndex];
                std::string meshName = aiMesh->mName.C_Str();
                std::string baseId = meshName.empty() ? "mesh_" + std::to_string(meshIndex) : meshName;

                std::string meshId = generateUniqueId(baseId);
                auto mesh = std::make_shared<Mesh>(mIdPrefix +meshId, "", meshName);

                std::vector<Vertex> vertices;
                vertices.reserve(aiMesh->mNumVertices);
                for (uint32_t j = 0; j < aiMesh->mNumVertices; j++) {
                    Vertex vertex;
                    vertex.position = aiVector3dToGlm(aiMesh->mVertices[j]);
                    vertex.normal = aiMesh->HasNormals() ? aiVector3dToGlm(aiMesh->mNormals[j]) : glm::vec3(0.0f, 1.0f, 0.0f);
                    vertex.texCoord = aiMesh->HasTextureCoords(0) ? glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f);
                    vertex.tangent = aiMesh->HasTangentsAndBitangents() ? aiVector3dToGlm(aiMesh->mTangents[j]) : glm::vec3(0.0f);
                    vertex.biNormal = aiMesh->HasTangentsAndBitangents() ? aiVector3dToGlm(aiMesh->mBitangents[j]) : glm::vec3(0.0f);
                    vertices.push_back(vertex);
                }
                mesh->setVertices(vertices);

                std::vector<uint32_t> indices;
                for (uint32_t j = 0; j < aiMesh->mNumFaces; j++) {
                    aiFace face = aiMesh->mFaces[j];
                    for (uint32_t k = 0; k < face.mNumIndices; k++) {
                        indices.push_back(face.mIndices[k]);
                    }
                }
                mesh->setIndices(indices);

                SubMeshData subMeshData;
                subMeshData.slot = 0;
                subMeshData.startIndex = 0;
                subMeshData.indexCount = indices.size();
                subMeshData.materialId = (aiMesh->mMaterialIndex < mMaterials.size()) ? mMaterials[aiMesh->mMaterialIndex]->getId() : "";
                calculateSubMeshBounds(*mesh, subMeshData);
                mesh->setSubMeshes({ subMeshData });

                calculateMeshBounds(*mesh);
                mMeshes.push_back(mesh);
            }
        }
    }

    void ModelParser::combineMeshes(const std::vector<uint32_t>& meshIndices, const aiScene* scene, Mesh& combinedMesh) {
        if (meshIndices.empty()) return;

        uint32_t totalVertices = 0, totalIndices = 0;
        for (uint32_t meshIndex : meshIndices) {
            aiMesh* mesh = scene->mMeshes[meshIndex];
            totalVertices += mesh->mNumVertices;
            totalIndices += mesh->mNumFaces * 3;
        }

        std::vector<Vertex> vertices;
        vertices.reserve(totalVertices);
        std::vector<uint32_t> indices;
        indices.reserve(totalIndices);
        std::vector<SubMeshData> subMeshes;
        subMeshes.reserve(meshIndices.size());

        uint32_t vertexOffset = 0, indexOffset = 0;
        uint32_t slot = 0;
        for (uint32_t meshIndex : meshIndices) {
            aiMesh* aiMesh = scene->mMeshes[meshIndex];
            SubMeshData subMeshData;
            subMeshData.slot = slot++;
            subMeshData.startIndex = indexOffset;
            subMeshData.materialId = (aiMesh->mMaterialIndex < mMaterials.size()) ? mMaterials[aiMesh->mMaterialIndex]->getId() : "";

            for (uint32_t j = 0; j < aiMesh->mNumVertices; j++) {
                Vertex vertex;
                vertex.position = aiVector3dToGlm(aiMesh->mVertices[j]);
                vertex.normal = aiMesh->HasNormals() ? aiVector3dToGlm(aiMesh->mNormals[j]) : glm::vec3(0.0f, 1.0f, 0.0f);
                vertex.texCoord = aiMesh->HasTextureCoords(0) ? glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f);
                vertex.tangent = aiMesh->HasTangentsAndBitangents() ? aiVector3dToGlm(aiMesh->mTangents[j]) : glm::vec3(0.0f);
                vertex.biNormal = aiMesh->HasTangentsAndBitangents() ? aiVector3dToGlm(aiMesh->mBitangents[j]) : glm::vec3(0.0f);
                vertices.push_back(vertex);
            }

            uint32_t subMeshIndexCount = 0;
            for (uint32_t j = 0; j < aiMesh->mNumFaces; j++) {
                aiFace face = aiMesh->mFaces[j];
                for (uint32_t k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k] + vertexOffset);
                    subMeshIndexCount++;
                }
            }

            subMeshData.indexCount = subMeshIndexCount;
            calculateSubMeshBounds(combinedMesh, subMeshData);
            subMeshes.push_back(subMeshData);

            vertexOffset += aiMesh->mNumVertices;
            indexOffset += subMeshIndexCount;
        }

        combinedMesh.setVertices(vertices);
        combinedMesh.setIndices(indices);
        combinedMesh.setSubMeshes(subMeshes);
        calculateMeshBounds(combinedMesh);
    }

    void ModelParser::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<EntityData> parent) {
        std::shared_ptr<EntityData> entity = parent;

        if (parent) {
            entity = std::make_shared<EntityData>();
            entity->name = node->mName.C_Str();
            entity->id = generateUniqueId(mIdPrefix+entity->name);
            glm::mat4 transform = aiMatrix4x4ToGlm(node->mTransformation);
            decomposeTransform(transform, entity->position, entity->rotation, entity->scale);
            parent->children.push_back(entity);
        }

        if (node->mNumMeshes > 0 && entity) {
            for (const auto& mesh : mMeshes) {
                bool belongsToNode = false;
                for (uint32_t i = 0; i < node->mNumMeshes; i++) {
                    aiMesh* nodeMesh = scene->mMeshes[node->mMeshes[i]];
                    if (mesh->getName() == nodeMesh->mName.C_Str() ||
                        (mesh->getName().empty() && mesh->getId().find(std::to_string(node->mMeshes[i])) != std::string::npos)) {
                        belongsToNode = true;
                        break;
                    }
                }
                if (belongsToNode) {
                    entity->meshId = mesh->getId();
                    for (const auto& subMeshData : mesh->getSubMeshes()) {
                        if (!subMeshData.materialId.empty()) {
                            entity->materialIds.push_back(subMeshData.materialId);
                        }
                    }
                    break;
                }
            }
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, entity);
        }
    }

    glm::vec3 ModelParser::aiVector3dToGlm(const aiVector3D& vec) {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    glm::vec3 ModelParser::aiColor3dToGlm(const aiColor3D& color) {
        return glm::vec3(color.r, color.g, color.b);
    }

    glm::mat4 ModelParser::aiMatrix4x4ToGlm(const aiMatrix4x4& mat) {
        return glm::mat4(
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
        );
    }

    void ModelParser::decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) {
        position = glm::vec3(transform[3]);
        scale.x = glm::length(glm::vec3(transform[0]));
        scale.y = glm::length(glm::vec3(transform[1]));
        scale.z = glm::length(glm::vec3(transform[2]));

        glm::mat4 rotMat = transform;
        rotMat[0] /= scale.x;
        rotMat[1] /= scale.y;
        rotMat[2] /= scale.z;
        rotMat[3] = glm::vec4(0, 0, 0, 1);

        rotation.y = asin(-rotMat[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(rotMat[1][2], rotMat[2][2]);
            rotation.z = atan2(rotMat[0][1], rotMat[0][0]);
        }
        else {
            rotation.x = atan2(-rotMat[2][1], rotMat[1][1]);
            rotation.z = 0;
        }
    }

    void ModelParser::saveMeshToXml(const Mesh& mesh, const std::filesystem::path& templateDir) {
        pugi::xml_document doc;
        auto root = doc.append_child("mesh");

        // Add submeshes node first
        if (!mesh.getSubMeshes().empty()) {
            auto subMeshesNode = root.append_child("submeshes");
            subMeshesNode.append_attribute("count") = mesh.getSubMeshes().size();
            for (size_t i = 0; i < mesh.getSubMeshes().size(); ++i) {
                const auto& subMeshData = mesh.getSubMeshes()[i];
                auto subMeshNode = subMeshesNode.append_child("submesh");
                subMeshNode.append_attribute("slot") = subMeshData.slot;
                subMeshNode.append_attribute("startIndex") = subMeshData.startIndex;
                subMeshNode.append_attribute("indexCount") = subMeshData.indexCount;
                subMeshNode.append_attribute("materialId") = subMeshData.materialId.c_str();

                auto subBoundsNode = subMeshNode.append_child("bounds");
                auto subMinNode = subBoundsNode.append_child("min");
                subMinNode.append_attribute("x") = subMeshData.bounds.aabbMin.x;
                subMinNode.append_attribute("y") = subMeshData.bounds.aabbMin.y;
                subMinNode.append_attribute("z") = subMeshData.bounds.aabbMin.z;
                auto subMaxNode = subBoundsNode.append_child("max");
                subMaxNode.append_attribute("x") = subMeshData.bounds.aabbMax.x;
                subMaxNode.append_attribute("y") = subMeshData.bounds.aabbMax.y;
                subMaxNode.append_attribute("z") = subMeshData.bounds.aabbMax.z;
            }
        }

        // Add bounds node second
        auto boundsNode = root.append_child("bounds");
        auto minNode = boundsNode.append_child("min");
        minNode.append_attribute("x") = mesh.getBounds().aabbMin.x;
        minNode.append_attribute("y") = mesh.getBounds().aabbMin.y;
        minNode.append_attribute("z") = mesh.getBounds().aabbMin.z;
        auto maxNode = boundsNode.append_child("max");
        maxNode.append_attribute("x") = mesh.getBounds().aabbMax.x;
        maxNode.append_attribute("y") = mesh.getBounds().aabbMax.y;
        maxNode.append_attribute("z") = mesh.getBounds().aabbMax.z;

        // Add positions node
        auto positionsNode = root.append_child("positions");
        positionsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto positionNode = positionsNode.append_child("position");
            positionNode.append_attribute("x") = vertex.position.x;
            positionNode.append_attribute("y") = vertex.position.y;
            positionNode.append_attribute("z") = vertex.position.z;
        }

        // Add normals node
        auto normalsNode = root.append_child("normals");
        normalsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto normalNode = normalsNode.append_child("normal");
            normalNode.append_attribute("x") = vertex.normal.x;
            normalNode.append_attribute("y") = vertex.normal.y;
            normalNode.append_attribute("z") = vertex.normal.z;
        }

        // Add texcoords node
        auto texCoordsNode = root.append_child("texcoords");
        texCoordsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto texCoordNode = texCoordsNode.append_child("texcoord");
            texCoordNode.append_attribute("u") = vertex.texCoord.x;
            texCoordNode.append_attribute("v") = vertex.texCoord.y;
        }

        // Add tangents node
        auto tangentsNode = root.append_child("tangents");
        tangentsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto tangentNode = tangentsNode.append_child("tangent");
            tangentNode.append_attribute("x") = vertex.tangent.x;
            tangentNode.append_attribute("y") = vertex.tangent.y;
            tangentNode.append_attribute("z") = vertex.tangent.z;
        }

        // Add bitangents node
        auto bitangentsNode = root.append_child("bitangents");
        bitangentsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto bitangentNode = bitangentsNode.append_child("bitangent");
            bitangentNode.append_attribute("x") = vertex.biNormal.x;
            bitangentNode.append_attribute("y") = vertex.biNormal.y;
            bitangentNode.append_attribute("z") = vertex.biNormal.z;
        }

        // Add indices node
        if (!mesh.getIndices().empty()) {
            auto indicesNode = root.append_child("indices");
            indicesNode.append_attribute("count") = mesh.getIndices().size();
            std::stringstream ss;
            for (size_t i = 0; i < mesh.getIndices().size(); ++i) {
                if (i > 0) ss << ",";
                ss << mesh.getIndices()[i];
            }
            indicesNode.text() = ss.str().c_str();
        }

        // Save to file
        std::string meshIdWithoutPrefix = mesh.getId();
        if (meshIdWithoutPrefix.find(mIdPrefix) == 0) {
            meshIdWithoutPrefix = meshIdWithoutPrefix.substr(mIdPrefix.length());
        }
        std::filesystem::path filepath = templateDir / (meshIdWithoutPrefix + ".mesh");
        doc.save_file(filepath.string().c_str());
    }

    void ModelParser::saveMaterialToXml(const Material& material, const std::filesystem::path& templateDir) {
        pugi::xml_document doc;

        // Add XML declaration
        auto decl = doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";

        auto root = doc.append_child("material");

        // Add shader node with generic shader (adjust as needed)
        auto shaderNode = root.append_child("shader");
        shaderNode.append_attribute("assetId") = "Standard";
        shaderNode.append_attribute("srcVert") = "../../Shaders/standard.vert";
        shaderNode.append_attribute("srcFrag") = "../../Shaders/standard.frag";

        // Add properties section
        auto propertiesNode = root.append_child("properties");

        // Serialize uniforms from the Material
        for (const auto& [name, value] : material.getUniforms()) {
            auto propertyNode = propertiesNode.append_child("property");
            propertyNode.append_attribute("name") = name.c_str();

            std::visit([&propertyNode](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    propertyNode.append_attribute("type") = "int";
                    propertyNode.append_attribute("value") = val;
                }
                else if constexpr (std::is_same_v<T, float>) {
                    propertyNode.append_attribute("type") = "float";
                    propertyNode.append_attribute("value") = std::to_string(val).c_str();
                }
                else if constexpr (std::is_same_v<T, glm::vec2>) {
                    propertyNode.append_attribute("type") = "vec2";
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << val.x << "," << val.y;
                    propertyNode.append_attribute("value") = ss.str().c_str();
                }
                else if constexpr (std::is_same_v<T, glm::vec3>) {
                    propertyNode.append_attribute("type") = "vec3";
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << val.x << "," << val.y << "," << val.z;
                    propertyNode.append_attribute("value") = ss.str().c_str();
                }
                else if constexpr (std::is_same_v<T, glm::vec4>) {
                    propertyNode.append_attribute("type") = "vec4";
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << val.x << "," << val.y << "," << val.z << "," << val.w;
                    propertyNode.append_attribute("value") = ss.str().c_str();
                }
                }, value);
        }


        auto texturesNode = root.append_child("textures");
        std::string materialIdWithoutPrefix = material.getId();
        if (materialIdWithoutPrefix.find(mIdPrefix) == 0) {
            materialIdWithoutPrefix = materialIdWithoutPrefix.substr(mIdPrefix.length());
        }
        std::filesystem::path filepath = templateDir / (materialIdWithoutPrefix + ".mat");
        doc.save_file(filepath.string().c_str());
    }

    void ModelParser::saveTemplateXml(const std::string& templateName, const std::filesystem::path& sourceFile, const std::filesystem::path& templateSrc) {
        pugi::xml_document doc;
        auto decl = doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "UTF-8";

        auto root = doc.append_child("template");
        root.append_attribute("name") = templateName.c_str();
        root.append_attribute("source") = sourceFile.string().c_str();

        auto assetsNode = root.append_child("assets");
        for (const auto& mesh : mMeshes) {
            auto meshNode = assetsNode.append_child("mesh");
            meshNode.append_attribute("assetId") = mesh->getId().c_str();
            std::string meshIdWithoutPrefix = mesh->getId();
            if (meshIdWithoutPrefix.find(mIdPrefix) == 0) {
                meshIdWithoutPrefix = meshIdWithoutPrefix.substr(mIdPrefix.length());
            }
            meshNode.append_attribute("src") = (meshIdWithoutPrefix + ".mesh").c_str();
        }

        for (const auto& material : mMaterials) {
            auto materialNode = assetsNode.append_child("material");
            materialNode.append_attribute("assetId") = material->getId().c_str();
            std::string materialIdWithoutPrefix = material->getId();
            if (materialIdWithoutPrefix.find(mIdPrefix) == 0) {
                materialIdWithoutPrefix = materialIdWithoutPrefix.substr(mIdPrefix.length());
            }
            materialNode.append_attribute("src") = (materialIdWithoutPrefix + ".mat").c_str();
        }

        auto entitiesNode = root.append_child("entities");
        for (const auto& entity : mTopLevelEntities) {
            writeEntityToXml(entitiesNode, entity);
        }

        doc.save_file((templateSrc.string()).c_str());
    }

    void ModelParser::writeEntityToXml(pugi::xml_node& parent, const std::shared_ptr<EntityData>& entity) {
        auto entityNode = parent.append_child("entity");
        entityNode.append_attribute("id") = entity->id.c_str();
        entityNode.append_attribute("name") = entity->name.c_str();

        std::stringstream posStream;
        posStream << std::fixed << std::setprecision(3)
            << entity->position.x << "," << entity->position.y << "," << entity->position.z;
        entityNode.append_attribute("position") = posStream.str().c_str();

        std::stringstream rotStream;
        rotStream << std::fixed << std::setprecision(3)
            << glm::degrees(entity->rotation.x) << ","
            << glm::degrees(entity->rotation.y) << ","
            << glm::degrees(entity->rotation.z);
        entityNode.append_attribute("rotation") = rotStream.str().c_str();

        std::stringstream scaleStream;
        scaleStream << std::fixed << std::setprecision(3)
            << entity->scale.x << "," << entity->scale.y << "," << entity->scale.z;
        entityNode.append_attribute("scale") = scaleStream.str().c_str();

        if (!entity->meshId.empty() || !entity->materialIds.empty()) {
            auto componentsNode = entityNode.append_child("components");
            auto rendererNode = componentsNode.append_child("renderer");
            if (!entity->meshId.empty()) {
                rendererNode.append_attribute("meshId") = entity->meshId.c_str();
            }
            if (!entity->materialIds.empty()) {
                if (entity->materialIds.size() == 1) {
                    rendererNode.append_attribute("materialId") = entity->materialIds[0].c_str();
                }
                else {
                    for (size_t i = 0; i < entity->materialIds.size(); ++i) {
                        auto materialNode = rendererNode.append_child("material");
                        materialNode.append_attribute("slot") = i;
                        materialNode.append_attribute("materialId") = entity->materialIds[i].c_str();
                    }
                }
            }
        }

        if (!entity->children.empty()) {
            auto childrenNode = entityNode.append_child("children");
            for (const auto& child : entity->children) {
                writeEntityToXml(childrenNode, child);
            }
        }
    }

    void ModelParser::calculateMeshBounds(Mesh& mesh) {
        if (mesh.getVertices().empty()) {
            mesh.setBounds(Bounds());
            return;
        }

        Bounds bounds;
        for (const auto& vertex : mesh.getVertices()) {
            bounds.Union(Bounds(vertex.position));
        }

        mesh.setBounds(bounds);
    }

    void ModelParser::calculateSubMeshBounds(const Mesh& mesh, SubMeshData& subMeshData) {
        if (mesh.getVertices().empty() || mesh.getIndices().empty() ||
            subMeshData.startIndex >= mesh.getIndices().size() ||
            subMeshData.startIndex + subMeshData.indexCount > mesh.getIndices().size()) {
            subMeshData.bounds = Bounds();
            return;
        }

        uint32_t firstIndex = mesh.getIndices()[subMeshData.startIndex];
        if (firstIndex >= mesh.getVertices().size()) {
            subMeshData.bounds = Bounds();
            return;
        }

        Bounds bounds(mesh.getVertices()[firstIndex].position);
        for (uint32_t i = subMeshData.startIndex; i < subMeshData.startIndex + subMeshData.indexCount; ++i) {
            uint32_t vertexIndex = mesh.getIndices()[i];
            if (vertexIndex < mesh.getVertices().size()) {
                bounds.Union(Bounds(mesh.getVertices()[vertexIndex].position));
            }
        }

        subMeshData.bounds = bounds;
    }

    void ModelParser::reset() {
        mMeshes.clear();
        mMaterials.clear();
        mTopLevelEntities.clear();
        mUsedIds.clear();
    }
}
#include "ModelParser.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace StrikeEngine {

    ModelParser::ModelParser() {
        mImporter.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
            aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    }

    ModelParser::~ModelParser() {
        reset();
    }

    bool ModelParser::parseModel(const std::string& modelPath, const std::string& templateDir) {
        reset();

        if (!std::filesystem::exists(modelPath)) {
            std::cerr << "Model file does not exist: " << modelPath << std::endl;
            return false;
        }

        std::string filename = std::filesystem::path(modelPath).stem().string();
        mIdPrefix = filename + ".";

        if (!std::filesystem::exists(templateDir)) {
            std::filesystem::create_directories(templateDir);
        }

        const aiScene* scene = mImporter.ReadFile(modelPath,
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

        mRootEntity = std::make_shared<EntityData>();
        mRootEntity->name = scene->mRootNode->mName.C_Str();
        mRootEntity->id = sanitizeId(mRootEntity->name);
        if (mRootEntity->id.empty()) {
            mRootEntity->id = "Root";
            mRootEntity->name = "Root";
        }

        glm::mat4 transform = aiMatrix4x4ToGlm(scene->mRootNode->mTransformation);
        decomposeTransform(transform, mRootEntity->position, mRootEntity->rotation, mRootEntity->scale);

        processNode(scene->mRootNode, scene, mRootEntity);

        for (const auto& mesh : mMeshes) {
            saveMeshToXml(*mesh, templateDir);
        }

        for (const auto& material : mMaterials) {
            saveMaterialToXml(*material, templateDir);
        }

        std::string templateName = std::filesystem::path(modelPath).stem().string();
        saveTemplateXml(templateName, modelPath, templateDir);

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
            std::string baseId = sanitizeId(matName);
            if (baseId.empty()) {
                baseId = "material_" + std::to_string(i);
            }
            std::string materialId = mIdPrefix + baseId;

            auto material = std::make_shared<Material>(materialId, matName);

            aiColor3D diffuse(1.0f, 1.0f, 1.0f);
            aiColor3D specular(1.0f, 1.0f, 1.0f);
            aiColor3D ambient(0.2f, 0.2f, 0.2f);
            aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
            aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
            aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
            material->setDiffuseColor(aiColor3dToGlm(diffuse));
            material->setSpecularColor(aiColor3dToGlm(specular));
            material->setAmbientColor(aiColor3dToGlm(ambient));

            float shininess = 32.0f;
            aiMat->Get(AI_MATKEY_SHININESS, shininess);
            material->setShininess(shininess);

            aiString texPath;
            if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
                material->setDiffuseTexture(texPath.C_Str());
            }
            if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS) {
                material->setSpecularTexture(texPath.C_Str());
            }
            if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS) {
                material->setNormalTexture(texPath.C_Str());
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
            if (meshIndices.size() == 1) {
                uint32_t meshIndex = meshIndices[0];
                aiMesh* aiMesh = scene->mMeshes[meshIndex];
                std::string meshName = aiMesh->mName.C_Str();
                std::string baseId = sanitizeId(meshName);
                if (baseId.empty()) {
                    baseId = "mesh_" + std::to_string(meshIndex);
                }
                std::string meshId = mIdPrefix + baseId;
                auto mesh = std::make_shared<Mesh>(meshId, meshName);

                std::vector<Vertex> vertices;
                vertices.reserve(aiMesh->mNumVertices);
                for (uint32_t j = 0; j < aiMesh->mNumVertices; j++) {
                    Vertex vertex;
                    vertex.position = aiVector3dToGlm(aiMesh->mVertices[j]);
                    vertex.normal = aiMesh->HasNormals() ? aiVector3dToGlm(aiMesh->mNormals[j]) : glm::vec3(0.0f, 1.0f, 0.0f);
                    vertex.texCoord = aiMesh->HasTextureCoords(0) ? glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f);
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
                subMeshData.startIndex = 0;
                subMeshData.indexCount = indices.size();
                subMeshData.materialId = (aiMesh->mMaterialIndex < mMaterials.size()) ? mMaterials[aiMesh->mMaterialIndex]->getId() : "";
                calculateSubMeshBounds(*mesh, subMeshData);
                mesh->setSubMeshes({ subMeshData });

                calculateMeshBounds(*mesh);
                mMeshes.push_back(mesh);
            }
            else if (meshIndices.size() > 1) {
                std::string combinedName = scene->mMeshes[meshIndices[0]]->mName.C_Str();
                std::string baseId = sanitizeId(combinedName);
                if (baseId.empty()) {
                    baseId = "combined_mesh_" + std::to_string(meshIndices[0]);
                }
                std::string meshId = mIdPrefix + baseId;
                auto combinedMesh = std::make_shared<Mesh>(meshId, combinedName);
                combineMeshes(meshIndices, scene, *combinedMesh);
                mMeshes.push_back(combinedMesh);
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
        for (uint32_t meshIndex : meshIndices) {
            aiMesh* aiMesh = scene->mMeshes[meshIndex];
            SubMeshData subMeshData;
            subMeshData.startIndex = indexOffset;
            subMeshData.materialId = (aiMesh->mMaterialIndex < mMaterials.size()) ? mMaterials[aiMesh->mMaterialIndex]->getId() : "";

            for (uint32_t j = 0; j < aiMesh->mNumVertices; j++) {
                Vertex vertex;
                vertex.position = aiVector3dToGlm(aiMesh->mVertices[j]);
                vertex.normal = aiMesh->HasNormals() ? aiVector3dToGlm(aiMesh->mNormals[j]) : glm::vec3(0.0f, 1.0f, 0.0f);
                vertex.texCoord = aiMesh->HasTextureCoords(0) ? glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f);
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
        std::shared_ptr<EntityData> entity;
        if (parent && node != scene->mRootNode) {
            entity = std::make_shared<EntityData>();
            entity->name = node->mName.C_Str();
            entity->id = sanitizeId(entity->name);
            if (entity->id.empty()) {
                static int nodeCounter = 0;
                entity->id = "node_" + std::to_string(nodeCounter++);
            }
            glm::mat4 transform = aiMatrix4x4ToGlm(node->mTransformation);
            decomposeTransform(transform, entity->position, entity->rotation, entity->scale);
            parent->children.push_back(entity);
        }
        else {
            entity = parent;
        }

        if (node->mNumMeshes > 0) {
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
                if (belongsToNode && entity) {
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

    void ModelParser::saveMeshToXml(const Mesh& mesh, const std::string& templateDir) {
        pugi::xml_document doc;
        auto root = doc.append_child("mesh");
        root.append_attribute("assetId") = mesh.getId().c_str();
        root.append_attribute("name") = mesh.getName().c_str();

        auto positionsNode = root.append_child("positions");
        positionsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto positionNode = positionsNode.append_child("position");
            positionNode.append_attribute("x") = vertex.position.x;
            positionNode.append_attribute("y") = vertex.position.y;
            positionNode.append_attribute("z") = vertex.position.z;
        }

        auto normalsNode = root.append_child("normals");
        normalsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto normalNode = normalsNode.append_child("normal");
            normalNode.append_attribute("x") = vertex.normal.x;
            normalNode.append_attribute("y") = vertex.normal.y;
            normalNode.append_attribute("z") = vertex.normal.z;
        }

        auto texCoordsNode = root.append_child("texcoords");
        texCoordsNode.append_attribute("count") = mesh.getVertices().size();
        for (const auto& vertex : mesh.getVertices()) {
            auto texCoordNode = texCoordsNode.append_child("texcoord");
            texCoordNode.append_attribute("u") = vertex.texCoord.x;
            texCoordNode.append_attribute("v") = vertex.texCoord.y;
        }

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

        if (!mesh.getSubMeshes().empty()) {
            auto subMeshesNode = root.append_child("submeshes");
            subMeshesNode.append_attribute("count") = mesh.getSubMeshes().size();
            for (size_t i = 0; i < mesh.getSubMeshes().size(); ++i) {
                const auto& subMeshData = mesh.getSubMeshes()[i];
                auto subMeshNode = subMeshesNode.append_child("SubMeshData");
                subMeshNode.append_attribute("slot") = i;
                subMeshNode.append_attribute("startIndex") = subMeshData.startIndex;
                subMeshNode.append_attribute("indexCount") = subMeshData.indexCount;
                subMeshNode.append_attribute("materialId") = subMeshData.materialId.c_str();

                auto subBoundsNode = subMeshNode.append_child("bounds");
                auto subMinNode = subBoundsNode.append_child("min");
                subMinNode.append_attribute("x") = subMeshData.boundsMin.x;
                subMinNode.append_attribute("y") = subMeshData.boundsMin.y;
                subMinNode.append_attribute("z") = subMeshData.boundsMin.z;
                auto subMaxNode = subBoundsNode.append_child("max");
                subMaxNode.append_attribute("x") = subMeshData.boundsMax.x;
                subMaxNode.append_attribute("y") = subMeshData.boundsMax.y;
                subMaxNode.append_attribute("z") = subMeshData.boundsMax.z;
            }
        }

        auto boundsNode = root.append_child("bounds");
        auto minNode = boundsNode.append_child("min");
        minNode.append_attribute("x") = mesh.getBoundsMin().x;
        minNode.append_attribute("y") = mesh.getBoundsMin().y;
        minNode.append_attribute("z") = mesh.getBoundsMin().z;
        auto maxNode = boundsNode.append_child("max");
        maxNode.append_attribute("x") = mesh.getBoundsMax().x;
        maxNode.append_attribute("y") = mesh.getBoundsMax().y;
        maxNode.append_attribute("z") = mesh.getBoundsMax().z;

        std::string meshIdWithoutPrefix = mesh.getId();
        if (meshIdWithoutPrefix.find(mIdPrefix) == 0) {
            meshIdWithoutPrefix = meshIdWithoutPrefix.substr(mIdPrefix.length());
        }
        std::string filepath = (std::filesystem::path(templateDir) / (meshIdWithoutPrefix + ".mesh")).string();
        doc.save_file(filepath.c_str());
    }

    void ModelParser::saveMaterialToXml(const Material& material, const std::string& templateDir) {
        pugi::xml_document doc;
        auto root = doc.append_child("material");
        root.append_attribute("assetId") = material.getId().c_str();
        root.append_attribute("name") = material.getName().c_str();

        auto diffuseNode = root.append_child("diffuse");
        diffuseNode.append_attribute("r") = material.getDiffuseColor().r;
        diffuseNode.append_attribute("g") = material.getDiffuseColor().g;
        diffuseNode.append_attribute("b") = material.getDiffuseColor().b;

        auto specularNode = root.append_child("specular");
        specularNode.append_attribute("r") = material.getSpecularColor().r;
        specularNode.append_attribute("g") = material.getSpecularColor().g;
        specularNode.append_attribute("b") = material.getSpecularColor().b;

        auto ambientNode = root.append_child("ambient");
        ambientNode.append_attribute("r") = material.getAmbientColor().r;
        ambientNode.append_attribute("g") = material.getAmbientColor().g;
        ambientNode.append_attribute("b") = material.getAmbientColor().b;

        auto shininessNode = root.append_child("shininess");
        shininessNode.append_attribute("value") = material.getShininess();

        if (!material.getDiffuseTexture().empty()) {
            auto diffuseTexNode = root.append_child("texture");
            diffuseTexNode.append_attribute("type") = "diffuse";
            diffuseTexNode.append_attribute("path") = material.getDiffuseTexture().c_str();
        }

        if (!material.getSpecularTexture().empty()) {
            auto specularTexNode = root.append_child("texture");
            specularTexNode.append_attribute("type") = "specular";
            specularTexNode.append_attribute("path") = material.getSpecularTexture().c_str();
        }

        if (!material.getNormalTexture().empty()) {
            auto normalTexNode = root.append_child("texture");
            normalTexNode.append_attribute("type") = "normal";
            normalTexNode.append_attribute("path") = material.getNormalTexture().c_str();
        }

        std::string materialIdWithoutPrefix = material.getId();
        if (materialIdWithoutPrefix.find(mIdPrefix) == 0) {
            materialIdWithoutPrefix = materialIdWithoutPrefix.substr(mIdPrefix.length());
        }
        std::string filepath = (std::filesystem::path(templateDir) / (materialIdWithoutPrefix + ".mat")).string();
        doc.save_file(filepath.c_str());
    }

    void ModelParser::saveTemplateXml(const std::string& templateName, const std::string& sourceFile, const std::string& templateDir) {
        pugi::xml_document doc;
        auto decl = doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "UTF-8";

        auto root = doc.append_child("template");
        root.append_attribute("name") = templateName.c_str();
        root.append_attribute("source") = sourceFile.c_str();

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
        if (mRootEntity) {
            writeEntityToXml(entitiesNode, mRootEntity);
        }

        std::string filepath = (std::filesystem::path(templateDir) / (templateName + ".xml")).string();
        doc.save_file(filepath.c_str());
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

    std::string ModelParser::sanitizeId(const std::string& name) {
        std::string result = name;
        std::replace_if(result.begin(), result.end(),
            [](char c) { return !std::isalnum(c) && c != '_' && c != '-'; }, '_');
        if (!result.empty() && std::isdigit(result[0])) {
            result = "_" + result;
        }
        auto end = std::unique(result.begin(), result.end(),
            [](char a, char b) { return a == '_' && b == '_'; });
        result.erase(end, result.end());
        return result;
    }

    void ModelParser::calculateMeshBounds(Mesh& mesh) {
        if (mesh.getVertices().empty()) {
            mesh.setBoundsMin(glm::vec3(0.0f));
            mesh.setBoundsMax(glm::vec3(0.0f));
            return;
        }

        glm::vec3 boundsMin = mesh.getVertices()[0].position;
        glm::vec3 boundsMax = mesh.getVertices()[0].position;
        for (const auto& vertex : mesh.getVertices()) {
            boundsMin = glm::min(boundsMin, vertex.position);
            boundsMax = glm::max(boundsMax, vertex.position);
        }

        mesh.setBoundsMin(boundsMin);
        mesh.setBoundsMax(boundsMax);
    }

    void ModelParser::calculateSubMeshBounds(const Mesh& mesh, SubMeshData& subMeshData) {
        if (mesh.getVertices().empty() || mesh.getIndices().empty() ||
            subMeshData.startIndex >= mesh.getIndices().size() ||
            subMeshData.startIndex + subMeshData.indexCount > mesh.getIndices().size()) {
            subMeshData.boundsMin = subMeshData.boundsMax = glm::vec3(0.0f);
            return;
        }

        uint32_t firstIndex = mesh.getIndices()[subMeshData.startIndex];
        if (firstIndex >= mesh.getVertices().size()) {
            subMeshData.boundsMin = subMeshData.boundsMax = glm::vec3(0.0f);
            return;
        }

        subMeshData.boundsMin = subMeshData.boundsMax = mesh.getVertices()[firstIndex].position;
        for (uint32_t i = subMeshData.startIndex; i < subMeshData.startIndex + subMeshData.indexCount; ++i) {
            uint32_t vertexIndex = mesh.getIndices()[i];
            if (vertexIndex < mesh.getVertices().size()) {
                const glm::vec3& position = mesh.getVertices()[vertexIndex].position;
                subMeshData.boundsMin = glm::min(subMeshData.boundsMin, position);
                subMeshData.boundsMax = glm::max(subMeshData.boundsMax, position);
            }
        }
    }

    void ModelParser::reset() {
        mMeshes.clear();
        mMaterials.clear();
        mRootEntity.reset();
    }
}
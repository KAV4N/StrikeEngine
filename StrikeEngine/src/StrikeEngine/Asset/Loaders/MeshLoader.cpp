#include "MeshLoader.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    MeshLoader::MeshLoader() : AssetLoader(Mesh::getStaticTypeName()) {}

    std::shared_ptr<Asset> MeshLoader::load(const std::string& id, const std::filesystem::path& filePath, bool async) {
        auto mesh = parseMeshFromXml(id, filePath);
        if (!mesh) {
            std::runtime_error("Failed to load mesh XML: " + filePath.string());
        }
        mesh->setLoadingState(AssetLoadingState::Loaded);
        if (!async) {
            std::lock_guard<std::mutex> lock(mMutex);
            LoadingTask task;
            task.id = mesh->getId();
            task.path = mesh->getPath();
            task.placeholderAsset = mesh;
            task.flagOnlyPostLoad = true;
            mLoadingTasks.emplace(task.id, std::move(task));
        }
        return mesh;
    }

    std::shared_ptr<Asset> MeshLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("assetId").as_string();
        std::filesystem::path src = node.attribute("src").as_string();
        std::string srcString = src.string();

        bool async = node.attribute("async").as_bool();
        
        if (assetId.empty() || src.empty()) {
            std::cerr << "Invalid mesh node: missing assetId or srcModel attribute" << std::endl;
            return nullptr;
        }

        src = resolvePath(src, basePath);

        if (async) 
            return AssetManager::get().loadMeshAsync(assetId, src);
        else 
            return AssetManager::get().loadMesh(assetId, src);
    }

    void MeshLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded)
    {
        auto placeholderAsset = std::dynamic_pointer_cast<Mesh>(placeholder);
        auto loadedAsset = std::dynamic_pointer_cast<Mesh>(loaded);
        placeholderAsset = std::move(loadedAsset);
        //*placeholderMesh = *loadedMesh;
    }

    std::shared_ptr<Mesh> MeshLoader::parseMeshFromXml(const std::string& id, const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            std::runtime_error("Mesh file does not exist " + filePath.string());
            //std::cerr << "Mesh file does not exist: " << filePath << std::endl;
            return nullptr;
        }

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filePath.c_str());
        if (!result) {
            std::cerr << "Failed to parse mesh XML file: " << filePath << " - " << result.description() << std::endl;
            return nullptr;
        }

        pugi::xml_node meshNode = doc.child("mesh");
        if (!meshNode) {
            std::cerr << "Invalid mesh XML file: missing 'mesh' root node in " << filePath << std::endl;
            return nullptr;
        }

        auto mesh = std::make_shared<Mesh>(id, addRootPrefix(filePath), filePath.stem().string());

        try {
            std::vector<Vertex> vertices = parseVertices(meshNode);
            mesh->setVertices(vertices);

            std::vector<uint32_t> indices = parseIndices(meshNode);
            mesh->setIndices(indices);

            std::vector<SubMeshData> subMeshes = parseSubMeshes(meshNode);
            mesh->setSubMeshes(subMeshes);

            pugi::xml_node boundsNode = meshNode.child("bounds");
            if (boundsNode) {
                glm::vec3 boundsMin = parseBounds(boundsNode, "min");
                glm::vec3 boundsMax = parseBounds(boundsNode, "max");
                mesh->setBounds(Bounds(boundsMin, boundsMax));
            }

            return mesh;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing mesh data from " << filePath << ": " << e.what() << std::endl;
            return nullptr;
        }
    }

    std::vector<Vertex> MeshLoader::parseVertices(const pugi::xml_node& meshNode) {
        std::vector<Vertex> vertices;
        pugi::xml_node positionsNode = meshNode.child("positions");
        pugi::xml_node normalsNode = meshNode.child("normals");
        pugi::xml_node texCoordsNode = meshNode.child("texcoords");

        if (!positionsNode) {
            throw std::runtime_error("Missing positions data in mesh XML");
        }

        size_t vertexCount = positionsNode.attribute("count").as_uint(0);
        if (vertexCount == 0) {
            return vertices;
        }

        vertices.reserve(vertexCount);

        std::vector<glm::vec3> positions;
        for (pugi::xml_node posNode : positionsNode.children("position")) {
            glm::vec3 pos(
                posNode.attribute("x").as_float(0.0f),
                posNode.attribute("y").as_float(0.0f),
                posNode.attribute("z").as_float(0.0f)
            );
            positions.push_back(pos);
        }

        std::vector<glm::vec3> normals;
        if (normalsNode) {
            for (pugi::xml_node normalNode : normalsNode.children("normal")) {
                glm::vec3 normal(
                    normalNode.attribute("x").as_float(0.0f),
                    normalNode.attribute("y").as_float(1.0f),
                    normalNode.attribute("z").as_float(0.0f)
                );
                normals.push_back(normal);
            }
        }

        std::vector<glm::vec2> texCoords;
        if (texCoordsNode) {
            for (pugi::xml_node texCoordNode : texCoordsNode.children("texcoord")) {
                glm::vec2 texCoord(
                    texCoordNode.attribute("u").as_float(0.0f),
                    texCoordNode.attribute("v").as_float(0.0f)
                );
                texCoords.push_back(texCoord);
            }
        }

        for (size_t i = 0; i < positions.size(); ++i) {
            Vertex vertex;
            vertex.position = positions[i];
            vertex.normal = (i < normals.size()) ? normals[i] : glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.texCoord = (i < texCoords.size()) ? texCoords[i] : glm::vec2(0.0f);
            vertices.push_back(vertex);
        }

        return vertices;
    }

    std::vector<uint32_t> MeshLoader::parseIndices(const pugi::xml_node& meshNode) {
        std::vector<uint32_t> indices;
        pugi::xml_node indicesNode = meshNode.child("indices");
        if (!indicesNode) {
            return indices;
        }

        size_t indexCount = indicesNode.attribute("count").as_uint(0);
        if (indexCount == 0) {
            return indices;
        }

        indices.reserve(indexCount);
        std::string indicesText = indicesNode.text().as_string();
        if (!indicesText.empty()) {
            std::stringstream ss(indicesText);
            std::string indexStr;
            while (std::getline(ss, indexStr, ',')) {
                indexStr.erase(std::remove_if(indexStr.begin(), indexStr.end(), ::isspace), indexStr.end());
                if (!indexStr.empty()) {
                    indices.push_back(std::stoul(indexStr));
                }
            }
        }

        return indices;
    }

    std::vector<SubMeshData> MeshLoader::parseSubMeshes(const pugi::xml_node& meshNode) {
        std::vector<SubMeshData> subMeshes;
        pugi::xml_node subMeshesNode = meshNode.child("submeshes");
        if (!subMeshesNode) {
            return subMeshes;
        }

        size_t subMeshCount = subMeshesNode.attribute("count").as_uint(0);
        if (subMeshCount == 0) {
            return subMeshes;
        }

        subMeshes.reserve(subMeshCount);
        for (pugi::xml_node subMeshNode : subMeshesNode.children("SubMeshData")) {
            SubMeshData subMeshData;
            subMeshData.slot = subMeshNode.attribute("slot").as_uint(0);
            subMeshData.startIndex = subMeshNode.attribute("startIndex").as_uint(0);
            subMeshData.indexCount = subMeshNode.attribute("indexCount").as_uint(0);
            subMeshData.materialId = subMeshNode.attribute("materialId").as_string();

            pugi::xml_node boundsNode = subMeshNode.child("bounds");
            if (boundsNode) {
                glm::vec3 boundsMin = parseBounds(boundsNode, "min");
                glm::vec3 boundsMax = parseBounds(boundsNode, "max");
                subMeshData.bounds = Bounds(boundsMin, boundsMax);
            }
            else {
                subMeshData.bounds = Bounds();
            }

            subMeshes.push_back(subMeshData);
        }

        return subMeshes;
    }

    glm::vec3 MeshLoader::parseBounds(const pugi::xml_node& boundsNode, const std::string& type) {
        pugi::xml_node node = boundsNode.child(type.c_str());
        if (!node) {
            return glm::vec3(0.0f);
        }

        return glm::vec3(
            node.attribute("x").as_float(0.0f),
            node.attribute("y").as_float(0.0f),
            node.attribute("z").as_float(0.0f)
        );
    }

    std::shared_ptr<Asset> MeshLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Mesh>(id, path, path.stem().string());
    }

}
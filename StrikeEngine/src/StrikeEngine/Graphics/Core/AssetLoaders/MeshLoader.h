#pragma once
#include "StrikeEngine/Graphics/Core/AssetLoaders/AssetLoader.h"
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <pugixml.hpp>
#include <glm/glm.hpp>
#include "StrikeEngine/Graphics/Core/Assets/Mesh.h"

namespace StrikeEngine {

    class MeshLoader : public AssetLoader {
    public:
        MeshLoader();
        ~MeshLoader() = default;

        std::shared_ptr<Asset> Load(const std::string& id, const std::filesystem::path& filePath) override;
        std::shared_ptr<Asset> LoadTask(const std::string& id, const std::filesystem::path& filePath) override;

    private:
        std::shared_ptr<Mesh> ParseMeshFromXML(const std::string& id, const std::filesystem::path& filePath);
        std::vector<Vertex> ParseVertices(const pugi::xml_node& meshNode);
        std::vector<uint32_t> ParseIndices(const pugi::xml_node& meshNode);
        std::vector<SubMeshData> ParseSubMeshes(const pugi::xml_node& meshNode);
        glm::vec3 ParseBounds(const pugi::xml_node& boundsNode, const std::string& type);
    };
}
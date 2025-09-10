#pragma once

#include "AssetLoader.h"
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <pugixml.hpp>
#include <glm/glm.hpp>
#include "StrikeEngine/Asset/Types/Mesh.h"

namespace StrikeEngine {

    class MeshLoader : public AssetLoader {
    public:
        MeshLoader();
        ~MeshLoader() = default;

        std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& filePath, bool async) override;
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    protected:
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;

    private:
        std::shared_ptr<Mesh> parseMeshFromXml(const std::string& id, const std::filesystem::path& filePath);
        std::vector<Vertex> parseVertices(const pugi::xml_node& meshNode);
        std::vector<uint32_t> parseIndices(const pugi::xml_node& meshNode);
        std::vector<SubMeshData> parseSubMeshes(const pugi::xml_node& meshNode);
        glm::vec3 parseBounds(const pugi::xml_node& boundsNode, const std::string& type);
    };

}
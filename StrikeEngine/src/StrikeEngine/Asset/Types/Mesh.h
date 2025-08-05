#pragma once
#include "StrikeEngine/Core/UUID.h"
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Asset.h"

namespace StrikeEngine {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    struct SubMeshData {
        uint32_t slot;
        uint32_t startIndex;
        uint32_t indexCount;
        std::string materialId;
        glm::vec3 boundsMin;
        glm::vec3 boundsMax;
    };

    class Mesh : public Asset {
    public:
        Mesh(const std::string& id, const std::filesystem::path& path, const std::string& name = "");

        static AssetType getStaticAssetType() { return AssetType::Mesh; }

        bool swapData(Asset& other) override;

        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndices() const;
        const std::vector<SubMeshData>& getSubMeshes() const;
        const glm::vec3& getBoundsMin() const;
        const glm::vec3& getBoundsMax() const;

        void setVertices(const std::vector<Vertex>& vertices);
        void setIndices(const std::vector<uint32_t>& indices);
        void setSubMeshes(const std::vector<SubMeshData>& subMeshes);
        void setBoundsMin(const glm::vec3& boundsMin);
        void setBoundsMax(const glm::vec3& boundsMax);
        void setBounds(const glm::vec3& boundsMin, const glm::vec3& boundsMax);

    private:
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        std::vector<SubMeshData> mSubMeshes;
        glm::vec3 mBoundsMin;
        glm::vec3 mBoundsMax;
    };
}
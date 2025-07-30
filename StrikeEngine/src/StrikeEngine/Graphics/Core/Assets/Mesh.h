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
        Mesh(const std::string& id, const std::string& name);

        static AssetType GetStaticAssetType() { return AssetType::MESH; }

        // Getters
        const std::string& GetName() const;
        const std::vector<Vertex>& GetVertices() const;
        const std::vector<uint32_t>& GetIndices() const;
        const std::vector<SubMeshData>& GetSubMeshes() const;
        const glm::vec3& GetBoundsMin() const;
        const glm::vec3& GetBoundsMax() const;

        // Setters
        void SetName(const std::string& name);
        void SetVertices(const std::vector<Vertex>& vertices);
        void SetIndices(const std::vector<uint32_t>& indices);
        void SetSubMeshes(const std::vector<SubMeshData>& subMeshes);
        void SetBoundsMin(const glm::vec3& boundsMin);
        void SetBoundsMax(const glm::vec3& boundsMax);

        void SetBounds(const glm::vec3& boundsMin, const glm::vec3& boundsMax);

    private:
        std::string m_name;
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<SubMeshData> m_subMeshes;
        glm::vec3 m_boundsMin;
        glm::vec3 m_boundsMax;
    };
}

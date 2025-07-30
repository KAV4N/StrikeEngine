#include "Mesh.h"

namespace StrikeEngine {

    Mesh::Mesh(const std::string& id, const std::string& name)
        : Asset(id), m_name(name), m_boundsMin(0.0f), m_boundsMax(0.0f) {
    }

    // Getters
    const std::string& Mesh::GetName() const { return m_name; }
    const std::vector<Vertex>& Mesh::GetVertices() const { return m_vertices; }
    const std::vector<uint32_t>& Mesh::GetIndices() const { return m_indices; }
    const std::vector<SubMeshData>& Mesh::GetSubMeshes() const { return m_subMeshes; }
    const glm::vec3& Mesh::GetBoundsMin() const { return m_boundsMin; }
    const glm::vec3& Mesh::GetBoundsMax() const { return m_boundsMax; }

    // Setters
    void Mesh::SetName(const std::string& name) { m_name = name; }
    void Mesh::SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }
    void Mesh::SetIndices(const std::vector<uint32_t>& indices) { m_indices = indices; }
    void Mesh::SetSubMeshes(const std::vector<SubMeshData>& subMeshes) { m_subMeshes = subMeshes; }
    void Mesh::SetBoundsMin(const glm::vec3& boundsMin) { m_boundsMin = boundsMin; }
    void Mesh::SetBoundsMax(const glm::vec3& boundsMax) { m_boundsMax = boundsMax; }

    void  Mesh::SetBounds(const glm::vec3& boundsMin, const glm::vec3& boundsMax) {
        m_boundsMax = boundsMax;
        m_boundsMin = boundsMin;
    }
}

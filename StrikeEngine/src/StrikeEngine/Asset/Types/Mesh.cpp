#include "Mesh.h"

namespace StrikeEngine {

    Mesh::Mesh(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(AssetType::Mesh, id, path, name),
        mBoundsMin(0.0f),
        mBoundsMax(0.0f) {
    }

    bool Mesh::swapData(Asset& other) {
        if (other.getType() != AssetType::Mesh) {
            return false;
        }

        Mesh* otherMesh = dynamic_cast<Mesh*>(&other);
        if (!otherMesh) {
            return false;
        }


        std::swap(mName, otherMesh->mName);
        std::swap(mVertices, otherMesh->mVertices);
        std::swap(mIndices, otherMesh->mIndices);
        std::swap(mSubMeshes, otherMesh->mSubMeshes);
        std::swap(mBoundsMin, otherMesh->mBoundsMin);
        std::swap(mBoundsMax, otherMesh->mBoundsMax);

        return true;
    }

    const std::vector<Vertex>& Mesh::getVertices() const {
        return mVertices;
    }

    const std::vector<uint32_t>& Mesh::getIndices() const {
        return mIndices;
    }

    const std::vector<SubMeshData>& Mesh::getSubMeshes() const {
        return mSubMeshes;
    }

    const glm::vec3& Mesh::getBoundsMin() const {
        return mBoundsMin;
    }

    const glm::vec3& Mesh::getBoundsMax() const {
        return mBoundsMax;
    }

    void Mesh::setVertices(const std::vector<Vertex>& vertices) {
        mVertices = vertices;
    }

    void Mesh::setIndices(const std::vector<uint32_t>& indices) {
        mIndices = indices;
    }

    void Mesh::setSubMeshes(const std::vector<SubMeshData>& subMeshes) {
        mSubMeshes = subMeshes;
    }

    void Mesh::setBoundsMin(const glm::vec3& boundsMin) {
        mBoundsMin = boundsMin;
    }

    void Mesh::setBoundsMax(const glm::vec3& boundsMax) {
        mBoundsMax = boundsMax;
    }

    void Mesh::setBounds(const glm::vec3& boundsMin, const glm::vec3& boundsMax) {
        mBoundsMin = boundsMin;
        mBoundsMax = boundsMax;
    }
}
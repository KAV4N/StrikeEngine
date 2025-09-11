#pragma once
#include "StrikeEngine/Core/UUID.h"
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Asset.h"
#include "StrikeEngine/Graphics/VertexArray.h"
#include "StrikeEngine/Graphics/VertexBuffer.h"
#include "StrikeEngine/Graphics/IndexBuffer.h"
#include <pugixml.hpp>

namespace StrikeEngine {

    class GraphicsAsset;

    struct Bounds {
        glm::vec3 aabbMax, aabbMin;
        Bounds() {
            float minNum = std::numeric_limits<float>::lowest();
            float maxNum = std::numeric_limits<float>::max();
            aabbMin = glm::vec3(maxNum, maxNum, maxNum);
            aabbMax = glm::vec3(minNum, minNum, minNum);
        }
        Bounds(const glm::vec3& p) { 
            aabbMax = p;
            aabbMin = p;
        }
        Bounds(const glm::vec3& min, const glm::vec3& max) { 
            aabbMax = max;
            aabbMin = min;
        }
        Bounds(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) { 
            aabbMax = glm::max(glm::max(a, b), c);
            aabbMin = glm::min(glm::min(a, b), c);
        }
        void Union(const Bounds& bounds) {
            aabbMax = glm::max(aabbMax, bounds.aabbMax);
            aabbMin = glm::min(aabbMin, bounds.aabbMin);
        }
        float area() {
            glm::vec3 e = aabbMax - aabbMin;
            return e.x * e.y + e.y * e.z + e.z * e.x;
        }
        glm::vec3 GetMidPoint() {
            return (aabbMax + aabbMin) / glm::vec3(2.0);
        }
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 biNormal;
    };

    struct SubMeshData {
        uint32_t slot;
        uint32_t startIndex;
        uint32_t indexCount;
        std::string materialId;
        Bounds bounds;
    };

    class Mesh : public Asset {
    public:
        Mesh(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        virtual ~Mesh();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "mesh";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        // Getters
        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndices() const;
        const std::vector<SubMeshData>& getSubMeshes() const;
        const Bounds& getBounds() const;

        void postload() override;

        const std::shared_ptr<VertexArray>& getVertexArray() const;
        const std::shared_ptr<VertexBuffer>& getVertexBuffer() const;
        const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;

        void setVertices(const std::vector<Vertex>& vertices);
        void setIndices(const std::vector<uint32_t>& indices);
        void setSubMeshes(const std::vector<SubMeshData>& subMeshes);
        void setBounds(const Bounds& bounds);

    private:
        // CPU data
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        std::vector<SubMeshData> mSubMeshes;
        Bounds mBounds;

        // GPU resources
        std::shared_ptr<VertexArray> mVertexArray;
        std::shared_ptr<VertexBuffer> mVertexBuffer;
        std::shared_ptr<IndexBuffer> mIndexBuffer;
    };
}

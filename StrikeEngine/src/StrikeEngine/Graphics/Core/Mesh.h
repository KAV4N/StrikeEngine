#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace StrikeEngine {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::ivec4 BoneIDs;
        glm::vec4 Weights;
    };

    struct Bone {
        std::string Name;
        int ParentIndex;
        glm::mat4 InverseBindMatrix;
    };

    class Skeleton {
    public:
        std::vector<Bone> Bones;
        std::unordered_map<std::string, int> BoneNameToIndex;
    };

    class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Skeleton> skeleton = nullptr);
        void Draw() const;
        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
        const Skeleton* GetSkeleton() const { return m_Skeleton.get(); }
        std::shared_ptr<Mesh> GetParent() const { return m_Parent; }
        const std::vector<std::shared_ptr<Mesh>>& GetChildren() const { return m_Children; }

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;

        std::unique_ptr<Skeleton> m_Skeleton;
        std::shared_ptr<Mesh> m_Parent;
        std::vector<std::shared_ptr<Mesh>> m_Children;

        friend class MeshManager;
    };
}
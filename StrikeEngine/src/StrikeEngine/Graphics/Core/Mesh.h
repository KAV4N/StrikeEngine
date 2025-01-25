#pragma once
#include <vector>
#include <memory>
#include "Material.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace StrikeEngine {
    class ModelManager;
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices,
            const std::vector<unsigned int>& indices,
            std::shared_ptr<Material> material);

    private:
        friend class ModelManager;
        void SetupMesh();

        std::vector<Vertex> m_Vertices;
        std::vector<unsigned int> m_Indices;
        std::shared_ptr<Material> m_Material;

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
    };

}
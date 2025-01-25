#include "Mesh.h"

namespace StrikeEngine {

    Mesh::Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        std::shared_ptr<Material> material)
        : m_Vertices(vertices), m_Indices(indices), m_Material(material) {
        SetupMesh();
    }

    void Mesh::SetupMesh() {
        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(),
            m_Vertices.size() * sizeof(Vertex));
        m_IBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());

        std::vector<VertexArray::VertexBufferElement> elements = {
            {GL_FLOAT, 3, GL_FALSE},  // Position
            {GL_FLOAT, 3, GL_FALSE},  // Normal
            {GL_FLOAT, 2, GL_FALSE}   // TexCoords
        };

        m_VAO->AddBuffer(*m_VBO, elements);
    }

}
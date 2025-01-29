#include "strikepch.h"
#include "Mesh.h"

namespace StrikeEngine {

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Skeleton> skeleton)
        : m_Vertices(vertices)
        , m_Indices(indices)
        , m_Skeleton(std::move(skeleton))
        , m_VAO(nullptr)
        , m_VBO(nullptr)
        , m_IBO(nullptr)
        , m_Parent(nullptr) {

        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
        m_IBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());

        std::vector<VertexBufferElement> elements = {
            {GL_FLOAT, 3, GL_FALSE},  // Position
            {GL_FLOAT, 3, GL_FALSE},  // Normal
            {GL_FLOAT, 2, GL_FALSE},  // TexCoords
            {GL_INT, 4, GL_FALSE},    // BoneIDs 
            {GL_FLOAT, 4, GL_FALSE}   // Weights 
        };

        m_VAO->AddBuffer(*m_VBO, elements);
    }

    void Mesh::Draw() const {
        m_VAO->Bind();
        m_IBO->Bind();
        glDrawElements(GL_TRIANGLES, m_IBO->GetCount(), GL_UNSIGNED_INT, nullptr);
        m_VAO->Unbind();
        m_IBO->Unbind();
    }
}
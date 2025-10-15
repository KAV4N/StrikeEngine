#include "Mesh.h"
#include <glad/glad.h>

namespace StrikeEngine {

    Mesh::Mesh(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name), mVAO(0), mVBO(0), mEBO(0)
    {
    }

    Mesh::~Mesh() {
        if (mVAO) {
            glDeleteVertexArrays(1, &mVAO);
        }
        if (mVBO) {
            glDeleteBuffers(1, &mVBO);
        }
        if (mEBO) {
            glDeleteBuffers(1, &mEBO);
        }
    }

    void Mesh::postLoad() {
        if (mVertices.empty() || mIndices.empty()) {
            return;
        }

        // Generate and bind Vertex Array Object (VAO)
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        // Generate and bind Vertex Buffer Object (VBO)
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

        // Generate and bind Element Buffer Object (EBO)
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), mIndices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers (position, normal, texCoord)
        // Position (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // TexCoord (location = 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);

        // Tangent (location = 3)
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        // BiNormal (location = 4)
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biNormal));
        glEnableVertexAttribArray(4);

        // Unbind VAO to prevent accidental modification
        glBindVertexArray(0);
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

    const Bounds& Mesh::getBounds() const {
        return mBounds;
    }

    GLuint Mesh::getVAO() const {
        return mVAO;
    }

    void Mesh::setVertices(const std::vector<Vertex>& vertices) {
        mVertices = vertices;
        if (mVBO) {
            glBindBuffer(GL_ARRAY_BUFFER, mVBO);
            glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    void Mesh::setIndices(const std::vector<uint32_t>& indices) {
        mIndices = indices;
        if (mEBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), mIndices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    void Mesh::setSubMeshes(const std::vector<SubMeshData>& subMeshes) {
        mSubMeshes = subMeshes;
    }

    void Mesh::setBounds(const Bounds& bounds) {
        mBounds = bounds;
    }
}
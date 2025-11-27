#include "Model.h"
#include <limits>
#include "StrikeEngine/Graphics/Renderer.h"

namespace StrikeEngine {

    // Mesh implementation
    Mesh::Mesh()
        : mVAO(0)
        , mVBO(0)
        , mEBO(0)
        , mInstanceVBO(0) 
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
        if (mInstanceVBO) {
            glDeleteBuffers(1, &mInstanceVBO);
        }
    }

    const std::vector<Vertex>& Mesh::getVertices() const {
        return mVertices;
    }

    const std::vector<uint32_t>& Mesh::getIndices() const {
        return mIndices;
    }

    const Bounds& Mesh::getBounds() const {
        return mBounds;
    }

    GLuint Mesh::getVAO() const {
        return mVAO;
    }

    GLuint Mesh::getInstanceVBO() const {
        return mInstanceVBO;
    }

    void Mesh::setVertices(const std::vector<Vertex>& vertices) {
        mVertices = vertices;
    }

    void Mesh::setIndices(const std::vector<uint32_t>& indices) {
        mIndices = indices;
    }

    void Mesh::setBounds(const Bounds& bounds) {
        mBounds = bounds;
    }

    void Mesh::setupGPUResources() {
        // Generate GPU buffers
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);

        // Vertex Buffer
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex),
                      mVertices.data(), GL_STATIC_DRAW);

        // Element Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t),
                      mIndices.data(), GL_STATIC_DRAW);

        // Vertex attributes
        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, position));
        
        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, normal));
        
        // TexCoord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, texCoord));
        
        // Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, tangent));

        glBindVertexArray(0);
        
        // Setup instance buffer with fixed size
        setupInstanceBuffer();
    }

    void Mesh::setupInstanceBuffer() {
        
        std::size_t vec4Size = sizeof(glm::vec4);
        std::size_t mat4Size = sizeof(glm::mat4); // 4 * vec4Size

        glBindVertexArray(mVAO);

        glGenBuffers(1, &mInstanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mInstanceVBO);
        
        glBufferData(GL_ARRAY_BUFFER,
                    10000 * mat4Size,  // Changed from sizeof(mat4Size)
                    nullptr,
                    GL_STATIC_DRAW);

        
        // Attribute location 4 – first vec4 (row 0)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(0 * vec4Size));


        // Attribute location 5 – second vec4 (row 1)
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(1 * vec4Size));


        // Attribute location 6 – third vec4 (row 2)
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));


        // Attribute location 7 – fourth vec4 (row 3)
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);

        glBindVertexArray(0);
    }

    void Mesh::updateInstanceBuffer(const glm::mat4* matrices, size_t count)
    {
        count = std::min(count, (size_t)Renderer::MAX_INSTANCES);

        glBindBuffer(GL_ARRAY_BUFFER, mInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        count * sizeof(glm::mat4),
                        matrices);
    }

    // Model implementation
    Model::Model(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
    {
        mNeedsPostLoad = true;
    }

    Model::~Model() {
    }

    const std::vector<std::shared_ptr<Mesh>>& Model::getMeshes() const {
        return mMeshes;
    }

    const Bounds& Model::getBounds() const {
        return mBounds;
    }

    std::shared_ptr<Mesh> Model::getMesh(uint32_t index) const {
        if (index < mMeshes.size()) {
            return mMeshes[index];
        }
        return nullptr;
    }

    uint32_t Model::getMeshCount() const {
        return static_cast<uint32_t>(mMeshes.size());
    }

    void Model::addMesh(const std::shared_ptr<Mesh>& mesh) {
        mMeshes.push_back(mesh);
        mBounds.unionBounds(mesh->getBounds());
    }

    void Model::clearMeshes() {
        mMeshes.clear();
        mBounds = Bounds();
    }

    void Model::postLoad() {
        // Setup GPU resources for all meshes
        for (auto& mesh : mMeshes) {
            mesh->setupGPUResources();
        }
    }
}
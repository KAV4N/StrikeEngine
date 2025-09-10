#include "Mesh.h"
#include <glad/glad.h>

namespace StrikeEngine {

    Mesh::Mesh(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name),
        mHasOpenGLResources(false) {
    }

    Mesh::~Mesh() {

    }


    void Mesh::postLoad() {
        createOpenGLResources();
    }

    void Mesh::createOpenGLResources() {
        if (mVertices.empty() || mIndices.empty()) {
            return;
        }

        mVertexBuffer = std::make_shared<VertexBuffer>();
        mIndexBuffer = std::make_shared<IndexBuffer>();
        mVertexArray = std::make_shared<VertexArray>();

        mVertexBuffer->setData(mVertices.data(), mVertices.size() * sizeof(Vertex));
        mIndexBuffer->setData(mIndices.data(), mIndices.size());

        mVertexArray->setVertexBuffer(*mVertexBuffer);
        mVertexArray->setIndexBuffer(*mIndexBuffer);

        mHasOpenGLResources = true;
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

    const std::shared_ptr<VertexArray>& Mesh::getVertexArray() const {
        return mVertexArray;
    }

    const std::shared_ptr<VertexBuffer>& Mesh::getVertexBuffer() const {
        return mVertexBuffer;
    }

    const std::shared_ptr<IndexBuffer>& Mesh::getIndexBuffer() const {
        return mIndexBuffer;
    }

    bool Mesh::hasOpenGLResources() const {
        return mHasOpenGLResources;
    }


    void Mesh::setVertices(const std::vector<Vertex>& vertices) {
        mVertices = vertices;
        if (mHasOpenGLResources && mVertexBuffer) {
            mVertexBuffer->setData(mVertices.data(), mVertices.size() * sizeof(Vertex));
        }
    }

    void Mesh::setIndices(const std::vector<uint32_t>& indices) {
        mIndices = indices;
        if (mHasOpenGLResources && mIndexBuffer) {
            mIndexBuffer->setData(mIndices.data(), mIndices.size());
        }
    }

    void Mesh::setSubMeshes(const std::vector<SubMeshData>& subMeshes) {
        mSubMeshes = subMeshes;
    }

    void Mesh::setBounds(const Bounds& bounds) {
        mBounds = bounds;
    }
}
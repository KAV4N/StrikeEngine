#pragma once

#include <vector>
#include <glad/glad.h>
#include <StrikeEngine/Renderer/RawModel.h>


namespace StrikeEngine {
    class ObjectLoader {
    public:
        RawModel* LoadModel(float vertices[], size_t verticesSize, int indices[], size_t indicesSize);

    private:
        int CreateVertexArrayObj();
        void StoreDataInAttribList(int attribNo, int elementCount, float data[], size_t dataSize);
        void BindIndicesBuffer(int indices[], size_t indicesSize);
        void Unbind();
        void Cleanup();

        std::vector<unsigned int> m_VertexArrayObjs;
        std::vector<unsigned int> m_VertexBufferObjs;
        std::vector<unsigned int> m_IndexBufferObjs;
    };
}



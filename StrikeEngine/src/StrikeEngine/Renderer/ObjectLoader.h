#pragma once

#include <vector>
#include <glad/glad.h>
#include <StrikeEngine/Renderer/Entity/Model.h>

namespace StrikeEngine {
    class ObjectLoader
    {
    public:
        ObjectLoader() {}
        virtual ~ObjectLoader() { cleanup(); }

        virtual Model* LoadModel(float vertices[], size_t vertexCount);

    private:
        virtual int CreateVertexArrayObj();
        virtual void StoreDataInAttribList(int attribNo, int elementCount, float data[], size_t dataSize);
        virtual void unbind();
        virtual void cleanup();

        std::vector<unsigned int> m_VertexArrayObjs;
        std::vector<unsigned int> m_VertexBufferObjs;
    };
}

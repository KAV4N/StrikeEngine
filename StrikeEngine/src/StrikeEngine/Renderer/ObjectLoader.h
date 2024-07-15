#pragma once

#include <vector>
#include <glad/glad.h>
#include "StrikeEngine/Scene/Model.h"
#include "StrikeEngine/Renderer/TextureManager.h"

namespace StrikeEngine {
    class ObjectLoader {
    public:
        Model* LoadModel(float vertices[], size_t verticesSize, int indices[], size_t indicesSize, Texture* texture);
        
        Model* LoadModel(float vertices[], size_t verticesSize, int indices[], size_t indicesSize, Texture* texture, float textureCoords[], size_t textureSize);
    private:
        int CreateVertexArrayObj();
        
        void StoreDataInAttribList(int attribNo, int elementCount, float data[], size_t dataSize);

        void BindIndicesBuffer(int indices[], size_t indicesSize);

        void Unbind();

        void Cleanup();
    private:
        std::vector<unsigned int> m_VertexArrayObjs;
        std::vector<unsigned int> m_VertexBufferObjs;
        std::vector<unsigned int> m_IndexBufferObjs;

    };
}



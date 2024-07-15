
#include "strikepch.h"
#include "ObjectLoader.h"

namespace StrikeEngine {
    Model* ObjectLoader::LoadModel(float vertices[], size_t verticesSize, int indices[], size_t indicesSize, Texture* texture)
    {
        int vao = CreateVertexArrayObj();
        BindIndicesBuffer(indices, indicesSize);
        StoreDataInAttribList(0, 3, vertices, verticesSize);

        Unbind();
        return new Model(vao, static_cast<int>(indicesSize), texture);
    }

    Model* ObjectLoader::LoadModel(float vertices[], size_t verticesSize, int indices[], size_t indicesSize, Texture* texture, float textureCoords[], size_t textureSize)
    {
        int vao = CreateVertexArrayObj();
        BindIndicesBuffer(indices, indicesSize);
        StoreDataInAttribList(0, 3, vertices, verticesSize);
        StoreDataInAttribList(1, 2, textureCoords, textureSize);
        Unbind();
        return new Model(vao, static_cast<int>(indicesSize), texture);
    }



    void ObjectLoader::BindIndicesBuffer(int indices[], size_t indexCount)
    {
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indices, GL_STATIC_DRAW);
        m_IndexBufferObjs.push_back(vbo);
    }

    int ObjectLoader::CreateVertexArrayObj()
    {
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        m_VertexArrayObjs.push_back(vao);
        return vao;
    }

    void ObjectLoader::StoreDataInAttribList(int attribNo, int elementCount, float data[], size_t dataSize)
    {
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, dataSize * sizeof(float), data, GL_STATIC_DRAW);
        glVertexAttribPointer(attribNo, elementCount, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(attribNo);
        m_VertexBufferObjs.push_back(vbo);
    }

    void ObjectLoader::Unbind()
    {
        glBindVertexArray(0);
    }

    void ObjectLoader::Cleanup()
    {
        for (unsigned int vao : m_VertexArrayObjs)
        {
            glDeleteVertexArrays(1, &vao);
        }

        for (unsigned int vbo : m_VertexBufferObjs)
        {
            glDeleteBuffers(1, &vbo);
        }

        m_VertexArrayObjs.clear();
        m_VertexBufferObjs.clear();
    }


}
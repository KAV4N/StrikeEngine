#include "strikepch.h"
#include "ObjectLoader.h"

#include <glad/glad.h>

namespace StrikeEngine {
    Model* ObjectLoader::LoadModel(float vertices[], size_t vertexCount)
    {
        int id = CreateVertexArrayObj();
        StoreDataInAttribList(0, 3, vertices, vertexCount * 3); 
        unbind();
        return new Model(id, static_cast<int>(vertexCount));
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

    void ObjectLoader::unbind()
    {
        glBindVertexArray(0);
    }

    void ObjectLoader::cleanup()
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

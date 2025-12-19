#include "strikepch.h"
#include "Skybox.h"
#include <glad/glad.h>

#include "StrikeEngine/Graphics/Shader.h"

namespace StrikeEngine {

    Skybox::Skybox()
        : mVAO(0)
        , mVBO(0)
        , mShader(nullptr)
    {
        mShader = ShaderManager::get().getShader("skybox.glsl");
        setupCube();
    }

    Skybox::~Skybox() {
        if (mVAO != 0) {
            glDeleteVertexArrays(1, &mVAO);
        }
        if (mVBO != 0) {
            glDeleteBuffers(1, &mVBO);
        }
    }



    void Skybox::setupCube() {
        std::array<float, 24> vertices = {
            //   Coordinates
            -1.0f, -1.0f,  1.0f, // 0
             1.0f, -1.0f,  1.0f, // 1
             1.0f, -1.0f, -1.0f, // 2
            -1.0f, -1.0f, -1.0f, // 3
            -1.0f,  1.0f,  1.0f, // 4
             1.0f,  1.0f,  1.0f, // 5
             1.0f,  1.0f, -1.0f, // 6
            -1.0f,  1.0f, -1.0f  // 7
        };

        std::array<GLuint, 36> indices = {
            // Front face (z = +1)
            0, 1, 5,
            5, 4, 0,
            // Back face (z = -1)
            2, 3, 7,
            7, 6, 2,
            // Left face (x = -1)
            3, 0, 4,
            4, 7, 3,
            // Right face (x = +1)
            1, 2, 6,
            6, 5, 1,
            // Top face (y = +1)
            4, 5, 6,
            6, 7, 4,
            // Bottom face (y = -1)
            1, 0, 3,
            3, 2, 1
        };

        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &ebo);
    }

}
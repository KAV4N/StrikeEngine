#include "Skybox.h"
#include <glad/glad.h>
#include <array>
#include "StrikeEngine/Asset/AssetManager.h"


namespace StrikeEngine {

    Skybox::Skybox()
        :
        mVAO(0)
        , mVBO(0)
        , mCubeMap(nullptr)
    {
        mShader = AssetManager::get().loadShader("EngineResources.DefaultSkyboxShader",
            "EngineResources/Shaders/Skybox.vert",
            "EngineResources/Shaders/Skybox.frag");
        mShader->postLoad();
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
            // Positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f
        };

        std::array<GLuint, 36> indices = {
            // Front
            0, 1, 2,
            2, 3, 0,
            // Right
            3, 2, 6,
            6, 7, 3,
            // Back
            7, 6, 5,
            5, 4, 7,
            // Left
            4, 5, 1,
            1, 0, 4,
            // Bottom
            1, 5, 6,
            6, 2, 1,
            // Top
            4, 0, 3,
            3, 7, 4
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


    void Skybox::bind() const {
        if (mShader->isReady() && mCubeMap->isReady()) {
            mShader->bind();
            mCubeMap->bind(0);
            glBindVertexArray(mVAO);
        }
    }

    void Skybox::unbind() const {

        if (mShader->isReady() && mCubeMap->isReady()) {
            glBindVertexArray(0);
            mCubeMap->unbind();
            mShader->unbind();
        }
    }
}
#include "Skybox.h"
#include <glad/glad.h>
#include <array>
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    Skybox::Skybox()
        : mVAO(0)
        , mVBO(0)
        , mShaderId("EngineResources.DefaultSkyboxShader")
        , mCubeMapId("")
        , mCubeMap(nullptr)
        , mShader(nullptr)
    {

        //load default skybox Shader
        mShader = AssetManager::get().loadShader(
            mShaderId,
            "EngineResources/Shaders/Skybox.vert",
            "EngineResources/Shaders/Skybox.frag"
        );
        if (mShader) {
            mShader->postLoad();
        }
        
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

    void Skybox::setCubeMap(const std::string& cubeMapId) {
        mCubeMapId = cubeMapId;
        mCubeMap = AssetManager::get().getCubeMap(mCubeMapId);

    }

    void Skybox::setShader(const std::string& shaderId) {
        mShaderId = shaderId;
        mShader = AssetManager::get().getShader(mShaderId);
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
            // Front face (towards the inside of the cube)
            3, 2, 6,
            6, 7, 3,
            // Back face
            0, 4, 5,
            5, 1, 0,
            // Left face
            0, 3, 7,
            7, 4, 0,
            // Right face
            1, 5, 6,
            6, 2, 1,
            // Top face
            4, 7, 6,
            6, 5, 4,
            // Bottom face
            0, 1, 2,
            2, 3, 0
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
        if (isShaderValid() && isCubeMapValid()) {
            mShader->bind();
            mCubeMap->bind(0);
            glBindVertexArray(mVAO);
        }
    }

    void Skybox::unbind() const {
        if (isShaderValid() && isCubeMapValid()) {
            glBindVertexArray(0);
            mCubeMap->unbind();
            mShader->unbind();
        }
    }
}
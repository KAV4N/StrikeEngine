// Renderer.h - No changes needed to header

// Renderer.cpp - Updated render() method
#include "Renderer.h"
#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include "GeometryRenderPass.h"
#include "SkyboxRenderPass.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    Renderer::Renderer() : mScreenVAO(0), mScreenVBO(0), mScreenEBO(0) {
        mFrameBuffer = std::make_unique<FrameBuffer>(mWidth, mHeight);
        init();
    }

    Renderer::~Renderer() {
        cleanup();
    }

    void Renderer::init() {
        addPass(std::move(std::make_unique<SkyboxRenderPass>(*this, "SkyboxPass")));
        addPass(std::move(std::make_unique<GeometryRenderPass>(*this, "GeometryPass")));

        setupScreenQuad();
        mScreenShader = AssetManager::get().loadShader(
            "EngineResources.finalPassShader",
            "EngineResources/Shaders/FinalPass.vert",
            "EngineResources/Shaders/FinalPass.frag"
        );
    }

    void Renderer::submit(const CameraRenderData& cameraData) {
        mCameraRenderData.push_back(cameraData);
    }

    void Renderer::addPass(std::unique_ptr<RenderPass> pass) {
        if (pass) {
            mPasses.push_back(std::move(pass));
        }
    }

    void Renderer::clearPasses() {
        mPasses.clear();
    }

    void Renderer::render() {
        mFrameBuffer->bind();
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& cameraData : mCameraRenderData) {
            const CameraComponent::Rect& viewportRect = cameraData.camera.getViewportRect();
            GLint viewportX = static_cast<GLint>(viewportRect.x * mWidth);
            GLint viewportY = static_cast<GLint>(viewportRect.y * mHeight);
            GLsizei viewportWidth = static_cast<GLsizei>(viewportRect.width * mWidth);
            GLsizei viewportHeight = static_cast<GLsizei>(viewportRect.height * mHeight);

            glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
            glEnable(GL_SCISSOR_TEST);
            glScissor(viewportX, viewportY, viewportWidth, viewportHeight);

          
            glClear(GL_DEPTH_BUFFER_BIT);

            for (auto& pass : mPasses) {
                if (pass && pass->isEnabled()) {
                    pass->execute(cameraData);
                }
                
            }

            glDisable(GL_SCISSOR_TEST);
        }

        mFrameBuffer->unBind();
        mCameraRenderData.clear();
    }

    void Renderer::display() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glViewport(0, 0, mWidth, mHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (mScreenShader && mScreenShader->isReady()) {
            mScreenShader->bind();
            mScreenShader->setInt("screenTexture", 0);
            mFrameBuffer->bindFramebufferTexture(0);

            glBindVertexArray(mScreenVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            mScreenShader->unbind();
        }
    }

    GLuint Renderer::getFinalTexture() const {
        return mFrameBuffer->getColorTextureID();
    }

    uint32_t Renderer::getWidth() const {
        return mWidth;
    }

    uint32_t Renderer::getHeight() const {
        return mHeight;
    }

    void Renderer::resize(uint32_t width, uint32_t height) {
        mWidth = width;
        mHeight = height;
        mFrameBuffer->resize(width, height);
        glViewport(0, 0, width, height);
    }

    void Renderer::setupScreenQuad() {
        // Full-screen quad vertices
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top-right
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Top-left
        };

        uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Generate and bind Vertex Array Object (VAO)
        glGenVertexArrays(1, &mScreenVAO);
        glBindVertexArray(mScreenVAO);

        // Generate and bind Vertex Buffer Object (VBO)
        glGenBuffers(1, &mScreenVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mScreenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Generate and bind Element Buffer Object (EBO)
        glGenBuffers(1, &mScreenEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // Unbind VAO to prevent accidental modification
        glBindVertexArray(0);
    }

    void Renderer::cleanup() {
        if (mScreenVAO) {
            glDeleteVertexArrays(1, &mScreenVAO);
        }
        if (mScreenVBO) {
            glDeleteBuffers(1, &mScreenVBO);
        }
        if (mScreenEBO) {
            glDeleteBuffers(1, &mScreenEBO);
        }
        mScreenShader.reset();
    }

}
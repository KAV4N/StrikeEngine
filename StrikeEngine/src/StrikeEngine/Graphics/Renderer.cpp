#include "strikepch.h"
#include "Renderer.h"

#include "RenderPasses/GeometryRenderPass.h"
#include "RenderPasses/LightCullingPass.h"
#include "RenderPasses/SkyboxRenderPass.h"
#include "RenderPasses/ShadowMapPass.h"

#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Graphics/Shader.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "StrikeEngine/Core/Profiler.h"

namespace StrikeEngine {

    const size_t Renderer::MAX_INSTANCES;

    Renderer::Renderer() {}

    Renderer::~Renderer() {
        cleanup();
    }

    void Renderer::init() {
        mFrameBuffer = std::make_unique<FrameBuffer>(mWidth, mHeight);
        
        addPass<ShadowMapPass>(PassStage::PreRender, *this);
        addPass<LightCullingPass>(PassStage::PreRender, *this);
        addPass<SkyboxRenderPass>(PassStage::MainRender, *this);
        addPass<GeometryRenderPass>(PassStage::MainRender, *this);
        
        setupScreenQuad();
        
        mScreenShader = ShaderManager::get().getShader("finalPass.glsl");
    }

    void Renderer::beginCamera(const CameraComponent& camera, const glm::vec3& position) {
        PROFILE_SCOPE("Renderer::beginCamera");

        mCurrentCameraData.clear();
        mCurrentCameraData.camera = camera;
        mCurrentCameraData.cameraPosition = position;
        
        mCameraActive = true;
    }

    void Renderer::endCamera() {
        PROFILE_SCOPE("Renderer::endCamera");
        
        if (!mCameraActive) return;

        renderCamera(mCurrentCameraData);
        mCurrentCameraData.clear();
        mCameraActive = false;
    }

    void Renderer::submitMesh(const std::shared_ptr<Mesh>& mesh,
                              const std::shared_ptr<Texture>& texture,
                              const glm::uvec3& color,
                              const glm::mat4& transform) {
        if (!mCameraActive || !mesh) return;

        PROFILE_SCOPE("Renderer::submitMesh");

        InstanceKey key{
            reinterpret_cast<uintptr_t>(mesh.get()),
            reinterpret_cast<uintptr_t>(texture.get()),
            static_cast<uint8_t>(glm::clamp(color.r, 0u, 255u)),
            static_cast<uint8_t>(glm::clamp(color.g, 0u, 255u)),
            static_cast<uint8_t>(glm::clamp(color.b, 0u, 255u))
        };

        auto& batch = mCurrentCameraData.instanceBatches[key];
        if (!batch.mesh) {
            batch.mesh = mesh;
            batch.texture = texture;
            batch.color = color;
            batch.worldMatrices.reserve(64);
        }

        batch.worldMatrices.push_back(transform);

        if (mCurrentCameraData.sunData.sun) {
            addToShadowBatch(mesh, transform);
        }
    }

    void Renderer::submitModel(const std::shared_ptr<Model>& model,
                               const std::shared_ptr<Texture>& texture,
                               const glm::uvec3& color,
                               const glm::mat4& transform) {
        if (!model) return;

        PROFILE_SCOPE("Renderer::submitModel");

        uint32_t meshCount = model->getMeshCount();
        for (uint32_t i = 0; i < meshCount; ++i) {
            auto mesh = model->getMesh(i);
            if (mesh) {
                submitMesh(mesh, texture, color, transform);
            }
        }
    }

    void Renderer::submitPointLight(const glm::vec3& position,
                                    const glm::uvec3& color,
                                    float intensity,
                                    float radius) {
        if (!mCameraActive) return;

        PointLight light;
        light.position = glm::vec4(position, 1.0f);
        light.color = glm::vec4(glm::vec3(color) / 255.0f, 1.0f);
        light.intensity = intensity;
        light.radius = radius;

        mCurrentCameraData.pointLights.push_back(light);
    }

    void Renderer::submitSun(Sun* sun) {
        if (!mCameraActive || !sun) return;

        PROFILE_SCOPE("Renderer::submitSun");
        mCurrentCameraData.sunData.sun = sun;
        mCurrentCameraData.sunData.lightSpaceMatrix = sun->calculateLightSpaceMatrix(mCurrentCameraData.cameraPosition, mCurrentCameraData.camera.getForward());
    }

    void Renderer::submitSkybox(const std::shared_ptr<CubeMap>& skybox) {
        if (!mCameraActive) return;
        mCurrentCameraData.mSkyboxTexture = skybox;
    }

    void Renderer::addToShadowBatch(const std::shared_ptr<Mesh>& mesh, 
                                    const glm::mat4& transform) {
        if (!mesh) return;

        ShadowInstanceKey key{
            reinterpret_cast<uintptr_t>(mesh.get())
        };

        auto& batch = mCurrentCameraData.sunData.shadowBatches[key];
        if (!batch.mesh) {
            batch.mesh = mesh;
            batch.worldMatrices.reserve(64);
        }

        batch.worldMatrices.push_back(transform);
    }

    void Renderer::renderCamera(const CameraRenderData& cameraData) {
        PROFILE_SCOPE("Renderer::renderCamera");

        preRender(cameraData);
        mainRender(cameraData);
        postRender(cameraData);
    }

    void Renderer::preRender(const CameraRenderData& cameraData) {
        PROFILE_SCOPE("Renderer::preRender");

        auto& passes = mStagePasses[static_cast<size_t>(PassStage::PreRender)];
        for (auto& pass : passes) {
            pass->execute(cameraData);
        }
    }

    void Renderer::mainRender(const CameraRenderData& cameraData) {
        PROFILE_SCOPE("Renderer::mainRender");

        mFrameBuffer->bind();

        const CameraComponent::Rect& viewportRect = cameraData.camera.getViewportRect();
        GLint viewportX = static_cast<GLint>(viewportRect.x * mWidth);
        GLint viewportY = static_cast<GLint>(viewportRect.y * mHeight);
        GLsizei viewportWidth = static_cast<GLsizei>(viewportRect.width * mWidth);
        GLsizei viewportHeight = static_cast<GLsizei>(viewportRect.height * mHeight);

        glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);
        glScissor(viewportX, viewportY, viewportWidth, viewportHeight);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto& passes = mStagePasses[static_cast<size_t>(PassStage::MainRender)];
        for (auto& pass : passes) {
            pass->execute(cameraData);
        }

        glDisable(GL_SCISSOR_TEST);
        mFrameBuffer->unBind();
    }

    void Renderer::postRender(const CameraRenderData& cameraData) {
        PROFILE_SCOPE("Renderer::postRender");

        auto& passes = mStagePasses[static_cast<size_t>(PassStage::PostRender)];
        for (auto& pass : passes) {
            pass->execute(cameraData);
        }
    }

    void Renderer::display() {
        PROFILE_SCOPE("Renderer::display");
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, mWidth, mHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (mScreenShader) {
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

    uint32_t Renderer::getWidth() const { return mWidth; }
    uint32_t Renderer::getHeight() const { return mHeight; }

    void Renderer::resize(uint32_t width, uint32_t height) {
        mWidth = width;
        mHeight = height;
        World::get().resize(width, height);
        mFrameBuffer->resize(width, height);
        glViewport(0, 0, width, height);
    }

    void Renderer::setupScreenQuad() {
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
        };

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &mScreenVAO);
        glBindVertexArray(mScreenVAO);

        glGenBuffers(1, &mScreenVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mScreenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &mScreenEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
    }

    void Renderer::cleanup() {
        if (mScreenVAO) glDeleteVertexArrays(1, &mScreenVAO);
        if (mScreenVBO) glDeleteBuffers(1, &mScreenVBO);
        if (mScreenEBO) glDeleteBuffers(1, &mScreenEBO);
        mScreenShader.reset();
        mScreenVAO = 0; mScreenVBO = 0; mScreenEBO = 0;
    }
}
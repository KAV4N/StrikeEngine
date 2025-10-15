#pragma once
#include <memory>
#include <vector>
#include "RenderPass.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/Types/Shader.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"
#include <GL/gl.h>

namespace StrikeEngine {

    struct CameraRenderData;

    class Renderer {
    public:
        static Renderer& get() {
            static Renderer instance;
            return instance;
        }

        void init();
        void submit(const CameraRenderData& cameraData);
        void addPass(std::unique_ptr<RenderPass> pass);
        void clearPasses();
        void render();
        void display();
        GLuint getFinalTexture() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        void resize(uint32_t width, uint32_t height);

    private:
        Renderer();
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void setupScreenQuad();
        void cleanup();

    private:
        std::vector<std::unique_ptr<RenderPass>> mPasses;
        std::vector<CameraRenderData> mCameraRenderData;
        std::unique_ptr<FrameBuffer> mFrameBuffer;

        std::shared_ptr<Shader> mScreenShader;
        GLuint mScreenVAO; // Vertex Array Object
        GLuint mScreenVBO; // Vertex Buffer Object
        GLuint mScreenEBO; // Element Buffer Object
        uint32_t mWidth = 1920;
        uint32_t mHeight = 1080;
    };

}
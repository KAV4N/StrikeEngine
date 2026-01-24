#pragma once
#include "RenderPasses/RenderPass.h"
#include "RenderData.h"

#include <memory>
#include <vector>
#include <array>

#include <GL/gl.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    class Shader;
    class FrameBuffer;
    class Mesh;
    class Texture;
    class Model;
    class CameraComponent;
    class CubeMap;
    class Sun;

    enum class PassStage {
        PreRender,
        MainRender,
        PostRender
    };

    class Renderer {
    public:
        static const size_t MAX_INSTANCES = 1024;

        static Renderer& get() {
            static Renderer instance;
            return instance;
        }

        void init();

        void beginCamera(const CameraComponent& camera, const glm::vec3& position);
        void endCamera(); 

        void submitMesh(const std::shared_ptr<Mesh>& mesh,
                       const std::shared_ptr<Texture>& texture,
                       const glm::uvec3& color,
                       const glm::mat4& transform);

        void submitModel(const std::shared_ptr<Model>& model,
                        const std::shared_ptr<Texture>& texture,
                        const glm::uvec3& color,
                        const glm::mat4& transform);

        void submitPointLight(const glm::vec3& position,
                             const glm::uvec3& color,
                             float intensity,
                             float radius);

        void submitSun(Sun* sun, const glm::mat4& lightSpaceMatrix);
        void submitSkybox(const std::shared_ptr<CubeMap>& skybox);

        void display();
        
        // Fog settings
        void setFogStart(float start) { mFogStart = start; }
        void setFogEnd(float end) { mFogEnd = end; }
        void setFogDensity(float density) { mFogDensity = density; }
        void setFogColor(const glm::uvec3& color) { mFogColor = color; }
        
        float getFogStart() const { return mFogStart; }
        float getFogEnd() const { return mFogEnd; }
        float getFogDensity() const { return mFogDensity; }
        glm::uvec3 getFogColor() const { return mFogColor; }

        template <typename T>
        T* getPass() const;

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
        void renderCamera(const CameraRenderData& cameraData);
        void preRender(const CameraRenderData& cameraData);
        void mainRender(const CameraRenderData& cameraData);
        void postRender(const CameraRenderData& cameraData);

        void addToShadowBatch(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform);

        template<typename T, typename... Args>
        T* addPass(PassStage stage, Args&&... args);

    private:
        std::array<std::vector<std::unique_ptr<RenderPass>>, 3> mStagePasses{
            std::vector<std::unique_ptr<RenderPass>>{},
            std::vector<std::unique_ptr<RenderPass>>{},
            std::vector<std::unique_ptr<RenderPass>>{}
        };

        CameraRenderData mCurrentCameraData;
        bool mCameraActive = false;

        std::unique_ptr<FrameBuffer> mFrameBuffer;

        std::shared_ptr<Shader> mScreenShader;
        GLuint mScreenVAO = 0;
        GLuint mScreenVBO = 0;
        GLuint mScreenEBO = 0;
        uint32_t mWidth = 1920;
        uint32_t mHeight = 1080;
        
        // Fog parameters
        float mFogStart = 10.0f;
        float mFogEnd = 100.0f;
        float mFogDensity = 0.0f;
        glm::uvec3 mFogColor = glm::uvec3(128, 153, 179); 
    };

    template<typename T, typename... Args>
    T* Renderer::addPass(PassStage stage, Args&&... args) {
        static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");

        auto pass = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = pass.get();
        pass->setup();
        mStagePasses[static_cast<size_t>(stage)].push_back(std::move(pass));
        return rawPtr;
    }

    template <typename T>
    T* Renderer::getPass() const {
        static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");

        for (const auto& stageVec : mStagePasses) {
            for (const auto& pass : stageVec) {
                if (auto* casted = dynamic_cast<T*>(pass.get())) {
                    return casted;
                }
            }
        }
        return nullptr;
    }
}
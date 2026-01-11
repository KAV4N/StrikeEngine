#pragma once
#include "RenderPasses/RenderPass.h"
#include "RenderCommands.h"

#include <memory>
#include <vector>
#include <array>

#include <GL/gl.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    class Shader;
    class FrameBuffer;
    class Mesh;
    class Material;
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

        // Camera control
        void beginCamera(const CameraComponent& camera, const glm::vec3& position);
        void endCamera(); 

        // Submission API
        void submitMesh(const std::shared_ptr<Mesh>& mesh,
                       const std::shared_ptr<Material>& material,
                       const glm::mat4& transform);

        void submitModel(const std::shared_ptr<Model>& model,
                        const std::shared_ptr<Material>& material,
                        const glm::mat4& transform);

        void submitPointLight(const glm::vec3& position,
                             const glm::vec3& color,
                             float intensity,
                             float radius,
                             float fallOff);

        void submitSun(Sun* sun);
        void submitSkybox(const std::shared_ptr<CubeMap>& skybox);

        // Display final result with post-processing
        void display();
        
        template<typename T, typename... Args>
        T* addPass(PassStage stage, Args&&... args);

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

    private:
        std::array<std::vector<std::unique_ptr<RenderPass>>, 3> mStagePasses{
            std::vector<std::unique_ptr<RenderPass>>{},
            std::vector<std::unique_ptr<RenderPass>>{},
            std::vector<std::unique_ptr<RenderPass>>{}
        };

        // Current camera being rendered
        CameraRenderData mCurrentCameraData;
        bool mCameraActive = false;

        std::unique_ptr<FrameBuffer> mFrameBuffer;

        std::shared_ptr<Shader> mScreenShader;
        GLuint mScreenVAO = 0;
        GLuint mScreenVBO = 0;
        GLuint mScreenEBO = 0;
        uint32_t mWidth = 1920;
        uint32_t mHeight = 1080;
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
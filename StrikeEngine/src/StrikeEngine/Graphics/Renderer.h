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

    /**
     * @brief Enumeration of rendering pipeline stages
     */
    enum class PassStage {
        PreRender,   /**< Early rendering stage (shadows, culling) */
        MainRender,  /**< Main geometry rendering stage */
        PostRender   /**< Post-processing stage */
    };

    /**
     * @brief Main rendering system for StrikeEngine
     * 
     * Manages the entire rendering pipeline including camera setup,
     * geometry submission, lighting, and post-processing effects.
     * 
     * @note Uses a modular render pass system for extensibility
     */
    class Renderer {
    public:
        static const size_t MAX_INSTANCES = 1024; /**< Maximum number of instances per batch */

        /**
         * @brief Get the singleton instance of Renderer
         * 
         * @return Reference to the Renderer instance
         */
        static Renderer& get() {
            static Renderer instance;
            return instance;
        }

        /**
         * @brief Initialize the renderer system
         * 
         * Sets up framebuffers, render passes, and screen quad for final rendering.
         */
        void init();

        /**
         * @brief Begin collecting render data for a camera
         * 
         * @param camera The camera component to render from
         * @param position World position of the camera
         */
        void beginCamera(const CameraComponent& camera, const glm::vec3& position);

        /**
         * @brief End camera rendering and submit collected data
         */
        void endCamera();

        /**
         * @brief Submit a mesh for rendering
         * 
         * @param mesh The mesh to render
         * @param texture The texture to apply
         * @param color Color tint (0-255 range)
         * @param transform World transformation matrix
         */
        void submitMesh(const std::shared_ptr<Mesh>& mesh,
                       const std::shared_ptr<Texture>& texture,
                       const glm::uvec3& color,
                       const glm::mat4& transform);

        /**
         * @brief Submit a model (collection of meshes) for rendering
         * 
         * @param model The model to render
         * @param texture The texture to apply to all meshes
         * @param color Color tint (0-255 range)
         * @param transform World transformation matrix
         */
        void submitModel(const std::shared_ptr<Model>& model,
                        const std::shared_ptr<Texture>& texture,
                        const glm::uvec3& color,
                        const glm::mat4& transform);

        /**
         * @brief Submit a point light for rendering
         * 
         * @param position World position of the light
         * @param color RGB color of the light (0-255 range)
         * @param intensity Light intensity multiplier
         * @param radius Light radius for attenuation
         */
        void submitPointLight(const glm::vec3& position,
                             const glm::uvec3& color,
                             float intensity,
                             float radius);

        /**
         * @brief Submit a sun/directional light for rendering
         * 
         * @param sun Pointer to the sun light source
         * @param lightSpaceMatrix Light space transformation matrix for shadows
         */
        void submitSun(Sun* sun, const glm::mat4& lightSpaceMatrix);

        /**
         * @brief Submit a skybox for rendering
         * 
         * @param skybox The skybox cubemap texture
         */
        void submitSkybox(const std::shared_ptr<CubeMap>& skybox);

        /**
         * @brief Display the final rendered frame
         * 
         * Renders the framebuffer contents to the screen with post-processing.
         */
        void display();
        
        /**
         * @brief Set the fog start distance
         * 
         * @param start Distance where fog begins
         */
        void setFogStart(float start) { mFogStart = start; }
        
        /**
         * @brief Set the fog end distance
         * 
         * @param end Distance where fog reaches maximum density
         */
        void setFogEnd(float end) { mFogEnd = end; }
        
        /**
         * @brief Set the fog density
         * 
         * @param density Fog density factor (0 = no fog, 1 = maximum fog)
         */
        void setFogDensity(float density) { mFogDensity = density; }
        
        /**
         * @brief Set the fog color
         * 
         * @param color RGB fog color (0-255 range)
         */
        void setFogColor(const glm::uvec3& color) { mFogColor = color; }
        
        /**
         * @brief Get the fog start distance
         * 
         * @return float Fog start distance
         */
        float getFogStart() const { return mFogStart; }
        
        /**
         * @brief Get the fog end distance
         * 
         * @return float Fog end distance
         */
        float getFogEnd() const { return mFogEnd; }
        
        /**
         * @brief Get the fog density
         * 
         * @return float Fog density factor
         */
        float getFogDensity() const { return mFogDensity; }
        
        /**
         * @brief Get the fog color
         * 
         * @return glm::uvec3 RGB fog color (0-255 range)
         */
        glm::uvec3 getFogColor() const { return mFogColor; }

        /**
         * @brief Get a render pass by type
         * 
         * @tparam T Type of render pass to retrieve
         * @return T* Pointer to the render pass, or nullptr if not found
         */
        template <typename T>
        T* getPass() const;

        /**
         * @brief Get the final rendered texture ID
         * 
         * @return GLuint OpenGL texture ID of the final render
         */
        GLuint getFinalTexture() const;
        
        /**
         * @brief Get the renderer width
         * 
         * @return uint32_t Width in pixels
         */
        uint32_t getWidth() const;
        
        /**
         * @brief Get the renderer height
         * 
         * @return uint32_t Height in pixels
         */
        uint32_t getHeight() const;
        
        /**
         * @brief Resize the renderer
         * 
         * @param width New width in pixels
         * @param height New height in pixels
         */
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

        /**
         * @brief Add a render pass to the pipeline
         * 
         * @tparam T Type of render pass to add
         * @tparam Args Argument types for render pass constructor
         * @param stage The pipeline stage to add the pass to
         * @param args Arguments for render pass constructor
         * @return T* Pointer to the created render pass
         */
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
        float mFogStart = 10.0f;                  /**< Distance where fog begins */
        float mFogEnd = 100.0f;                   /**< Distance where fog reaches maximum density */
        float mFogDensity = 0.0f;                 /**< Fog density factor (0-1) */
        glm::uvec3 mFogColor = glm::uvec3(128, 153, 179); /**< Fog color (0-255 range) */
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
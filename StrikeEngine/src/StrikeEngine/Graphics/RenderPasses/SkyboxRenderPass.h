#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>

namespace StrikeEngine {

    class Skybox;
    class Renderer;

    /**
     * @brief Skybox rendering pass
     * 
     * Renders environment cubemap as a skybox using specialized
     * OpenGL state for proper depth handling.
     * 
     * @note Uses GL_LEQUAL depth function and front-face culling for skybox rendering
     */
    class SkyboxRenderPass : public RenderPass {
    public:
        /**
         * @brief Construct a new SkyboxRenderPass object
         * 
         * @param renderer Reference to the main renderer
         */
        SkyboxRenderPass(Renderer& renderer);
        
        /**
         * @brief Destroy the SkyboxRenderPass object
         */
        ~SkyboxRenderPass() override;

        /**
         * @brief Setup skybox render pass resources
         */
        void setup() override;
        
        /**
         * @brief Clean up skybox render pass resources
         */
        void cleanup() override;
        
        /**
         * @brief Execute the skybox rendering pass
         * 
         * Renders cubemap skybox with special depth handling
         * 
         * @param cameraData Camera rendering data for this frame
         */
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();

        Renderer& mRenderer;           /**< Reference to main renderer */
        std::unique_ptr<Skybox> mSkybox; /**< Skybox geometry and shader manager */
    };
}
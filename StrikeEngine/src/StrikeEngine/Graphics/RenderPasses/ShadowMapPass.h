#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>
#include <memory>

namespace Strike {

    class FrameBuffer;
    class Renderer;

    /**
     * @brief Shadow mapping pass for sun/directional light shadows
     * 
     * Renders depth maps from the sun's perspective for shadow calculations.
     * Uses a framebuffer with depth texture attachment.
     * 
     * @note Uses front-face culling to reduce peter-panning artifacts
     */
    class ShadowMapPass : public RenderPass {
    public:
        /**
         * @brief Construct a new ShadowMapPass object
         * 
         * @param renderer Reference to the main renderer
         */
        ShadowMapPass(Renderer& renderer);
        
        /**
         * @brief Destroy the ShadowMapPass object
         */
        ~ShadowMapPass() override;

        /**
         * @brief Setup shadow map pass resources
         */
        void setup() override;
        
        /**
         * @brief Clean up shadow map pass resources
         */
        void cleanup() override;
        
        /**
         * @brief Execute the shadow mapping pass
         * 
         * Renders depth map from sun's perspective
         * 
         * @param cameraData Camera rendering data for this frame
         */
        void execute(const CameraRenderData& cameraData) override;

        /**
         * @brief Get the shadow map texture ID
         * 
         * @return GLuint OpenGL texture ID of the shadow map
         */
        GLuint getShadowMapTexture() const;

    private:
        void setupOpenGLState();
        void restoreOpenGLState(uint32_t width, uint32_t height);

    private:
        Renderer& mRenderer;                     /**< Reference to main renderer */
        std::unique_ptr<FrameBuffer> mShadowMap; /**< Framebuffer for shadow map rendering */
        uint32_t mShadowMapSize = 2048;          /**< Resolution of shadow map (square) */
    };
}
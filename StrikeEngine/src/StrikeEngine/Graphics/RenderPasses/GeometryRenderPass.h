#pragma once

#include "RenderPass.h"
#include <memory>
#include <vector>
#include <glad/glad.h>

namespace Strike {

    class Renderer;
    class Shader;

    /**
     * @brief Main geometry rendering pass
     * 
     * Handles rendering of all meshes with instanced rendering,
     * lighting (sun + point lights), shadows, and textures.
     * 
     * @note Uses clustered forward rendering for efficient lighting
     */
    class GeometryRenderPass : public RenderPass {
    public:
        /**
         * @brief Construct a new GeometryRenderPass object
         * 
         * @param renderer Reference to the main renderer
         */
        GeometryRenderPass(Renderer& renderer);
        
        /**
         * @brief Destroy the GeometryRenderPass object
         */
        ~GeometryRenderPass() override;

        /**
         * @brief Setup geometry render pass resources
         */
        void setup() override;
        
        /**
         * @brief Clean up geometry render pass resources
         */
        void cleanup() override;
        
        /**
         * @brief Execute the geometry rendering pass
         * 
         * Renders all instance batches with lighting and shadows
         * 
         * @param cameraData Camera rendering data for this frame
         */
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();
        void renderInstanceBatch(const InstanceBatch& batch,
                                const CameraRenderData& cameraData);
        void createWhiteTexture();
        void destroyWhiteTexture();

    private:
        Renderer& mRenderer;               /**< Reference to main renderer */
        std::shared_ptr<Shader> mShader;   /**< Geometry rendering shader */
        GLuint mWhiteTextureID;            /**< Default white texture for untextured meshes */
    };
}
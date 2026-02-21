#pragma once
#include "RenderPass.h"
#include <memory>
#include <glm/glm.hpp>


namespace Strike {

    class Shader;
    class Renderer;

    /**
     * @brief Render pass for visualizing collision boxes as wireframes
     * 
     * This pass renders physics colliders as green wireframe boxes
     * for debugging purposes, showing their position, rotation, and size.
     * 
     * @note Only renders when PhysicsComponent is present on entities
     */
    class CollisionRenderPass : public RenderPass {
    public:
        /**
         * @brief Construct a new CollisionRenderPass object
         * 
         * @param renderer Reference to the main renderer
         */
        CollisionRenderPass(Renderer& renderer);
        
        /**
         * @brief Destroy the CollisionRenderPass object
         */
        ~CollisionRenderPass() override;

        /**
         * @brief Setup collision render pass resources
         */
        void setup() override;
        
        /**
         * @brief Clean up collision render pass resources
         */
        void cleanup() override;
        
        /**
         * @brief Execute the collision rendering pass
         * 
         * Renders all physics colliders as wireframe boxes
         * 
         * @param cameraData Camera rendering data for this frame
         */
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();
        void setupBoxGeometry();
        void renderBox(const glm::mat4& modelMatrix, const glm::mat4& viewProj);

        Renderer& mRenderer;                     /**< Reference to main renderer */
        std::shared_ptr<Shader> mShader;         /**< Wireframe rendering shader */
        GLuint mBoxVAO, mBoxVBO, mBoxEBO;        /**< OpenGL buffers for box geometry */
        uint32_t mBoxIndexCount;                 /**< Number of indices in box geometry */
    };
}
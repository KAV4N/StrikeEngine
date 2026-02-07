#pragma once
#include <string>

#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include <glad/glad.h>

namespace StrikeEngine {

    /**
     * @brief Base class for all render passes in the rendering pipeline
     * 
     * Render passes are modular components that handle specific rendering tasks
     * like shadow mapping, geometry rendering, post-processing, etc.
     * 
     * @note Each pass is executed in a specific pipeline stage (PreRender, MainRender, PostRender)
     */
    class RenderPass {
    public:
        /**
         * @brief Construct a new RenderPass object
         * 
         * @param name Unique name identifier for the render pass
         */
        RenderPass(const std::string& name) : mName(name) {}
        
        /**
         * @brief Destroy the RenderPass object
         */
        virtual ~RenderPass() = default;

        /**
         * @brief Execute the render pass
         * 
         * @param cameraData Camera rendering data for this frame
         */
        virtual void execute(const CameraRenderData& cameraData) = 0;

        /**
         * @brief Get the name of this render pass
         * 
         * @return const std::string& The render pass name
         */
        const std::string& getName() const { return mName; }

        /**
         * @brief Setup resources for this render pass
         * 
         * Called once during initialization to set up shaders, buffers, etc.
         */
        virtual void setup() {}
        
        /**
         * @brief Clean up resources for this render pass
         * 
         * Called during shutdown to free allocated resources.
         */
        virtual void cleanup() {}

    protected:
        std::string mName; /**< Unique identifier for this render pass */
    };
}
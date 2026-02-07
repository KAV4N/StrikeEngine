#pragma once
#include <cstdint>
#include <glad/glad.h>

namespace StrikeEngine {
    /**
     * @brief OpenGL framebuffer wrapper for off-screen rendering
     * 
     * Provides functionality for creating and managing OpenGL framebuffers
     * with color and depth-stencil attachments.
     * 
     * @note Supports resizing and binding to texture slots
     */
    class FrameBuffer
    {
    public:
        /**
         * @brief Construct a new FrameBuffer object
         * 
         * @param width The width of the framebuffer
         * @param height The height of the framebuffer
         */
        FrameBuffer(uint32_t width, uint32_t height);
        
        /**
         * @brief Destroy the FrameBuffer object
         * 
         * Cleans up OpenGL resources.
         */
        ~FrameBuffer();
        
        /**
         * @brief Get the color attachment texture ID
         * 
         * @return GLuint OpenGL texture ID of the color attachment
         */
        GLuint getColorTextureID() const { return mColorTexture; }
        
        /**
         * @brief Get the depth-stencil attachment texture ID
         * 
         * @return GLuint OpenGL texture ID of the depth-stencil attachment
         */
        GLuint getDepthTextureID() const { return mDepthTexture; }
        
        /**
         * @brief Get the framebuffer width
         * 
         * @return uint32_t Width in pixels
         */
        uint32_t getWidth() const { return mWidth; }
        
        /**
         * @brief Get the framebuffer height
         * 
         * @return uint32_t Height in pixels
         */
        uint32_t getHeight() const { return mHeight; }
        
        /**
         * @brief Clear the framebuffer
         * 
         * Clears both color and depth buffers.
         */
        void clear();
        
        /**
         * @brief Bind the framebuffer for rendering
         */
        void bind();
        
        /**
         * @brief Unbind the framebuffer
         */
        void unBind();
        
        /**
         * @brief Resize the framebuffer
         * 
         * @param width New width in pixels
         * @param height New height in pixels
         */
        void resize(uint32_t width, uint32_t height);
        
        /**
         * @brief Bind the framebuffer's color texture to a texture slot
         * 
         * @param slot The texture slot to bind to (0-31)
         */
        void bindFramebufferTexture(int slot);
        
        /**
         * @brief Bind the framebuffer's depth texture to a texture slot
         * 
         * @param slot The texture slot to bind to (0-31)
         */
        void bindFramebufferDepthTexture(int slot);

    private:
        GLuint mRendererID = 0;
        GLuint mColorTexture = 0;
        GLuint mDepthTexture = 0;
        uint32_t mWidth;
        uint32_t mHeight;
        
    private:
        void invalidate();
        static const uint32_t sMaxFramebufferSize = 8192;
    };
}
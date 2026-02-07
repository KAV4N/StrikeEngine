#pragma once

#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace StrikeEngine {

    /**
     * @brief Manages the OpenGL graphics context
     * 
     * Handles initialization of OpenGL through GLAD and provides
     * context management for GLFW windows.
     */
    class GraphicsContext {
    public:
        /**
         * @brief Construct a new GraphicsContext object
         * 
         * @param windowHandle Pointer to the GLFW window
         */
        GraphicsContext(GLFWwindow* windowHandle);
        
        /**
         * @brief Destroy the GraphicsContext object
         */
        ~GraphicsContext() = default;

        /**
         * @brief Initialize the graphics context
         * 
         * Sets up GLAD and verifies OpenGL version requirements.
         */
        void init();
        
        /**
         * @brief Swap the front and back buffers
         */
        void swapBuffers();

        /**
         * @brief Create a GraphicsContext instance
         * 
         * @param window Pointer to the GLFW window
         * @return std::unique_ptr<GraphicsContext> Unique pointer to the created context
         */
        static std::unique_ptr<GraphicsContext> create(GLFWwindow* window);

    private:
        GLFWwindow* mWindowHandle;
    };
}
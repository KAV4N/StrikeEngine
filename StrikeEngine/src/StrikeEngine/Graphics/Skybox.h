#pragma once

#include "StrikeEngine/Graphics/Shader.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace StrikeEngine {

    /**
     * @brief Skybox rendering component
     * 
     * Manages skybox geometry and shaders for environment rendering.
     */
    class Skybox {
    public:
        /**
         * @brief Construct a new Skybox object
         * 
         * Sets up cube geometry and loads the skybox shader.
         */
        Skybox();
        
        /**
         * @brief Destroy the Skybox object
         * 
         * Cleans up OpenGL resources.
         */
        ~Skybox();

        /**
         * @brief Bind the skybox for rendering
         */
        void bind() const;
        
        /**
         * @brief Unbind the skybox
         */
        void unbind() const;

        /**
         * @brief Get the skybox shader
         * 
         * @return std::shared_ptr<Shader> Shared pointer to the skybox shader
         */
        std::shared_ptr<Shader> getShader() const { return mShader; }

        /**
         * @brief Get the skybox VAO
         * 
         * @return GLuint OpenGL vertex array object ID
         */
        GLuint getVAO() const { return mVAO; }

    private:
        /**
         * @brief Set up the cube geometry for skybox rendering
         */
        void setupCube();

        std::shared_ptr<Shader> mShader; /**< Skybox rendering shader */

        GLuint mVAO; /**< Vertex array object for skybox cube */
        GLuint mVBO; /**< Vertex buffer object for skybox cube */
        GLuint mEBO; /**< Element buffer object for skybox cube */
    };
}
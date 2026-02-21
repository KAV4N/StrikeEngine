#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <fstream>
#include <filesystem>

namespace Strike {
    class ShaderManager;
    
    /**
     * @brief Base shader class for OpenGL shader programs
     * 
     * Manages vertex and fragment shaders, uniform setting, and binding.
     */
    class Shader {
    public:
        /**
         * @brief Construct a new Shader object
         * 
         * @param id Unique identifier for the shader
         */
        Shader(const std::string& id);
        
        /**
         * @brief Destroy the Shader object
         * 
         * Cleans up OpenGL resources.
         */
        virtual ~Shader();

        /**
         * @brief Bind the shader for rendering
         */
        void bind() const;
        
        /**
         * @brief Unbind the shader
         */
        void unbind() const;

        /**
         * @brief Set an integer uniform
         * 
         * @param name Name of the uniform
         * @param value Integer value
         */
        void setInt(const std::string& name, int value);
        
        /**
         * @brief Set an integer array uniform
         * 
         * @param name Name of the uniform
         * @param values Pointer to integer array
         * @param count Number of elements in array
         */
        void setIntArray(const std::string& name, int* values, uint32_t count);
        
        /**
         * @brief Set a float uniform
         * 
         * @param name Name of the uniform
         * @param value Float value
         */
        void setFloat(const std::string& name, float value);
        
        /**
         * @brief Set a vec2 uniform
         * 
         * @param name Name of the uniform
         * @param value glm::vec2 value
         */
        void setVec2(const std::string& name, const glm::vec2& value);
        
        /**
         * @brief Set a vec3 uniform
         * 
         * @param name Name of the uniform
         * @param value glm::vec3 value
         */
        void setVec3(const std::string& name, const glm::vec3& value);
        
        /**
         * @brief Set a vec4 uniform
         * 
         * @param name Name of the uniform
         * @param value glm::vec4 value
         */
        void setVec4(const std::string& name, const glm::vec4& value);
        
        /**
         * @brief Set a mat4 uniform
         * 
         * @param name Name of the uniform
         * @param value glm::mat4 value
         */
        void setMat4(const std::string& name, const glm::mat4& value);

        /**
         * @brief Set a uvec2 uniform (unsigned integer vector)
         * 
         * @param name Name of the uniform
         * @param value glm::uvec2 value
         */
        void setUVec2(const std::string& name, const glm::uvec2& value);
        
        /**
         * @brief Set a uvec3 uniform (unsigned integer vector)
         * 
         * @param name Name of the uniform
         * @param value glm::uvec3 value
         */
        void setUVec3(const std::string& name, const glm::uvec3& value);

        /**
         * @brief Get the OpenGL program ID
         * 
         * @return GLuint OpenGL program ID
         */
        GLuint getRendererId() const { return mRendererId; }
        
        /**
         * @brief Get the shader identifier
         * 
         * @return const std::string& Shader ID
         */
        const std::string& getId() const { return mId; }

    protected:
        friend class ShaderManager;
        GLuint compileShader(GLenum type, const std::string& source);
        void linkProgram();
        GLint getUniformLocation(const std::string& name);
        void cleanup();

        GLuint mRendererId;
        std::unordered_map<std::string, GLint> mUniformLocationCache;
        std::string mId;
    };

    /**
     * @brief Compute shader specialization
     * 
     * Extends Shader for compute shader functionality.
     */
    class ComputeShader : public Shader {
    public:
        /**
         * @brief Construct a new ComputeShader object
         * 
         * @param id Unique identifier for the compute shader
         */
        ComputeShader(const std::string& id);
        
        /**
         * @brief Destroy the ComputeShader object
         */
        ~ComputeShader() = default;

        /**
         * @brief Dispatch the compute shader
         * 
         * @param numGroupsX Number of work groups in X dimension
         * @param numGroupsY Number of work groups in Y dimension
         * @param numGroupsZ Number of work groups in Z dimension
         */
        void dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ);
        
        /**
         * @brief Wait for compute shader execution to finish
         * 
         * Inserts a memory barrier for shader storage operations.
         */
        void waitFinish();
    };

    /**
     * @brief Manages loading and accessing shaders
     * 
     * Singleton class that loads all shaders from files and provides
     * access to them by ID.
     */
    class ShaderManager {
    public:
        /**
         * @brief Get the singleton instance of ShaderManager
         * 
         * @return Reference to the ShaderManager instance
         */
        static ShaderManager& get();

        /**
         * @brief Load all shaders from the shaders directory
         * 
         * Scans res/shaders directory for .glsl and .shader files
         * and loads them into memory.
         */
        void loadAllShaders();
        
        /**
         * @brief Get a shader by ID
         * 
         * @param id Shader identifier (filename)
         * @return std::shared_ptr<Shader> Shared pointer to the shader
         */
        std::shared_ptr<Shader> getShader(const std::string& id);
        
        /**
         * @brief Get a compute shader by ID
         * 
         * @param id Compute shader identifier (filename)
         * @return std::shared_ptr<ComputeShader> Shared pointer to the compute shader
         */
        std::shared_ptr<ComputeShader> getComputeShader(const std::string& id);

        /**
         * @brief Check if a shader exists
         * 
         * @param id Shader identifier to check
         * @return true If the shader exists
         * @return false Otherwise
         */
        bool hasShader(const std::string& id) const;
        
        /**
         * @brief Clear all loaded shaders
         */
        void clear();

    private:
        ShaderManager();
        ~ShaderManager() = default;

        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;

        /**
         * @brief Structure containing shader source code
         */
        struct ShaderSources {
            std::string vertexSource;   /**< Vertex shader source code */
            std::string fragmentSource; /**< Fragment shader source code */
            std::string computeSource;  /**< Compute shader source code */
        };
        
        std::string readFileToString(const std::filesystem::path& filepath);

        ShaderSources parseShaderSource(const std::string& source);
        
        std::shared_ptr<Shader> createShaderFromSource(const std::string& id, const ShaderSources& sources);
        std::shared_ptr<ComputeShader> createComputeShaderFromSource(const std::string& id, const std::string& source);

        std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;
    };
}
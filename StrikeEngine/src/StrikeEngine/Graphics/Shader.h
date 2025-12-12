#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>

namespace StrikeEngine {
    class ShaderManager;
    
    class Shader {
    public:
        Shader(const std::string& id);
        virtual ~Shader();

        void bind() const;
        void unbind() const;

        void setInt(const std::string& name, int value);
        void setIntArray(const std::string& name, int* values, uint32_t count);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setMat4(const std::string& name, const glm::mat4& value);

        GLuint getRendererId() const { return mRendererId; }
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

    class ComputeShader : public Shader {
    public:
        ComputeShader(const std::string& id);
        ~ComputeShader() = default;

        void dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ);
        void waitFinish();
    };

    class ShaderManager {
    public:
        static ShaderManager& get();

        void loadAllShaders();
        
        std::shared_ptr<Shader> getShader(const std::string& id);
        std::shared_ptr<ComputeShader> getComputeShader(const std::string& id);

        bool hasShader(const std::string& id) const;
        void clear();

    private:
        ShaderManager();
        ~ShaderManager() = default;

        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;

        struct ShaderSources {
            std::string vertexSource;
            std::string fragmentSource;
            std::string computeSource;
        };
        
        ShaderSources parseShaderSource(const std::string& source);
        
        std::shared_ptr<Shader> createShaderFromSource(const std::string& id, const ShaderSources& sources);
        std::shared_ptr<ComputeShader> createComputeShaderFromSource(const std::string& id, const std::string& source);

        std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;
    };
}
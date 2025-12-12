#include "Shader.h"
#include "GenShaders.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {

    Shader::Shader(const std::string& id)
        : mId(id), mRendererId(0) {
    }

    Shader::~Shader() {
        cleanup();
    }

    void Shader::cleanup() {
        if (mRendererId != 0) {
            glDeleteProgram(mRendererId);
            mRendererId = 0;
        }
        mUniformLocationCache.clear();
    }

    void Shader::bind() const {
        if (mRendererId != 0) {
            glUseProgram(mRendererId);
        }
    }

    void Shader::unbind() const {
        glUseProgram(0);
    }

    GLuint Shader::compileShader(GLenum type, const std::string& source) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, '\0');
            glGetShaderInfoLog(shader, length, nullptr, log.data());

            std::string shaderType;
            if (type == GL_VERTEX_SHADER) shaderType = "VERTEX";
            else if (type == GL_FRAGMENT_SHADER) shaderType = "FRAGMENT";
            else if (type == GL_COMPUTE_SHADER) shaderType = "COMPUTE";
            
            const std::string errorMsg = "Shader compilation error (" + shaderType + ") for " + mId + ":\n" + log;

            glDeleteShader(shader);
            throw std::runtime_error(errorMsg);
        }

        return shader;
    }

    void Shader::linkProgram() {
        glLinkProgram(mRendererId);

        GLint success;
        glGetProgramiv(mRendererId, GL_LINK_STATUS, &success);
        if (!success) {
            GLint length;
            glGetProgramiv(mRendererId, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, '\0');
            glGetProgramInfoLog(mRendererId, length, nullptr, log.data());

            const std::string errorMsg = "Shader linking error for " + mId + ":\n" + log;
            throw std::runtime_error(errorMsg);
        }
    }

    GLint Shader::getUniformLocation(const std::string& name) {
        auto it = mUniformLocationCache.find(name);
        if (it != mUniformLocationCache.end()) {
            return it->second;
        }

        GLint location = glGetUniformLocation(mRendererId, name.c_str());
        mUniformLocationCache[name] = location;
        return location;
    }

    void Shader::setInt(const std::string& name, int value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1i(location, value);
        }
    }

    void Shader::setIntArray(const std::string& name, int* values, uint32_t count) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1iv(location, count, values);
        }
    }

    void Shader::setFloat(const std::string& name, float value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1f(location, value);
        }
    }

    void Shader::setVec2(const std::string& name, const glm::vec2& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::setVec4(const std::string& name, const glm::vec4& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& value) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    ComputeShader::ComputeShader(const std::string& id)
        : Shader(id) {
    }

    void ComputeShader::dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) {
        bind();
        glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
    }

    void ComputeShader::waitFinish(){
         glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // ShaderManager Implementation

    ShaderManager::ShaderManager() {
        loadAllShaders();
    }

    ShaderManager& ShaderManager::get() {
        static ShaderManager instance;
        return instance;
    }

    ShaderManager::ShaderSources ShaderManager::parseShaderSource(const std::string& source) {
        ShaderSources result;
        
        std::istringstream stream(source);
        std::string line;
        std::string* currentSource = nullptr;
        
        while (std::getline(stream, line)) {
            // Check for #type directive
            if (line.find("#type") != std::string::npos) {
                if (line.find("VERTEX") != std::string::npos) {
                    currentSource = &result.vertexSource;
                } else if (line.find("FRAGMENT") != std::string::npos) {
                    currentSource = &result.fragmentSource;
                } else if (line.find("COMPUTE") != std::string::npos) {
                    currentSource = &result.computeSource;
                }
                continue; // Skip the #type line itself
            }
            
            // Append line to current source
            if (currentSource != nullptr) {
                *currentSource += line + "\n";
            }
        }
        
        return result;
    }

    std::shared_ptr<Shader> ShaderManager::createShaderFromSource(const std::string& id, const ShaderSources& sources) {
        try {
            auto shader = std::make_shared<Shader>(id);

            if (sources.vertexSource.empty() || sources.fragmentSource.empty()) {
                throw std::runtime_error("Missing vertex or fragment shader source");
            }

            GLuint vertexShader = shader->compileShader(GL_VERTEX_SHADER, sources.vertexSource);
            GLuint fragmentShader = shader->compileShader(GL_FRAGMENT_SHADER, sources.fragmentSource);

            shader->mRendererId = glCreateProgram();
            glAttachShader(shader->mRendererId, vertexShader);
            glAttachShader(shader->mRendererId, fragmentShader);

            shader->linkProgram();

            glDetachShader(shader->mRendererId, vertexShader);
            glDetachShader(shader->mRendererId, fragmentShader);
            
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            return shader;
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create shader " << id << ": " << e.what() << std::endl;
            return nullptr;
        }
    }

    std::shared_ptr<ComputeShader> ShaderManager::createComputeShaderFromSource(const std::string& id, const std::string& source) {
        try {
            auto shader = std::make_shared<ComputeShader>(id);

            if (source.empty()) {
                throw std::runtime_error("Empty compute shader source");
            }

            GLuint computeShader = shader->compileShader(GL_COMPUTE_SHADER, source);
            
            shader->mRendererId = glCreateProgram();
            glAttachShader(shader->mRendererId, computeShader);
            
            shader->linkProgram();
            
            glDetachShader(shader->mRendererId, computeShader);
            glDeleteShader(computeShader);

            return shader;
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create compute shader " << id << ": " << e.what() << std::endl;
            return nullptr;
        }
    }

    void ShaderManager::loadAllShaders() {
        std::cout << "Loading shaders from GenShaders.h..." << std::endl;
        
        for (const auto& [filename, shaderSource] : shaderMap) {
            
            std::cout << "Loading shader: " << filename << std::endl;
            
            ShaderSources sources = parseShaderSource(shaderSource);

            if (!sources.computeSource.empty()) {
                auto computeShader = createComputeShaderFromSource(filename, sources.computeSource);
                if (computeShader) {
                    mShaders[filename] = computeShader;
                    std::cout << "  -> Loaded as compute shader" << std::endl;
                }
            } else if (!sources.vertexSource.empty() && !sources.fragmentSource.empty()) {
                auto shader = createShaderFromSource(filename, sources);
                if (shader) {
                    mShaders[filename] = shader;
                    std::cout << "  -> Loaded as vertex/fragment shader" << std::endl;
                }
            } else {
                std::cerr << "  -> Warning: Shader " << filename << " has incomplete sources" << std::endl;
            }
        }
        
        std::cout << "Loaded " << mShaders.size() << " shaders total" << std::endl;
    }

    std::shared_ptr<Shader> ShaderManager::getShader(const std::string& id) {
        auto it = mShaders.find(id);
        if (it != mShaders.end()) {
            auto shader = std::dynamic_pointer_cast<Shader>(it->second);
            if (shader) {
                return shader;
            }
        }
        return nullptr;
    }

    std::shared_ptr<ComputeShader> ShaderManager::getComputeShader(const std::string& id) {
        auto it = mShaders.find(id);
        if (it != mShaders.end()) {
            auto shader = std::dynamic_pointer_cast<ComputeShader>(it->second);
            if (shader) {
                return shader;
            }
        }
        return nullptr;
    }

    bool ShaderManager::hasShader(const std::string& id) const {
        return mShaders.find(id) != mShaders.end();
    }

    void ShaderManager::clear() {
        mShaders.clear();
    }
}
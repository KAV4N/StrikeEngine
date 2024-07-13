#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>

namespace StrikeEngine {

    class Shader {
    public:
        Shader(const std::string& filepath);
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;

    private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    private:
        GLuint m_ProgramID;
    };

}

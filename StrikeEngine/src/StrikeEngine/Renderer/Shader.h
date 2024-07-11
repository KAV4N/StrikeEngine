#pragma once

#include <string>

namespace StrikeEngine {

    class Shader {
    public:
        Shader(unsigned int type);
        ~Shader();

        bool LoadFromFile(const std::string& filePath, unsigned int& programID);
        bool Compile();
        bool LinkProgram();

    private:
        unsigned int m_Type;
        unsigned int m_ShaderID;
        unsigned int m_ProgramID;
        std::string m_SourceCode;
    };

}

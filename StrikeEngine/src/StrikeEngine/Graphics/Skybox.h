#pragma once


#include "StrikeEngine/Graphics/Shader.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace StrikeEngine {

    class Skybox {
    public:
        Skybox();
        ~Skybox();

        void bind() const;
        void unbind() const;


        std::shared_ptr<Shader> getShader() const { return mShader; }

        
        GLuint getVAO() const { return mVAO; }

    private:
        void setupCube();


        std::shared_ptr<Shader> mShader;

        GLuint mVAO;
        GLuint mVBO;
        GLuint mEBO;
    };
}
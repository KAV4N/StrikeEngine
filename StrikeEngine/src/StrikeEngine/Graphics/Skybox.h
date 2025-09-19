#pragma once

#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Types/Shader.h"

#include <glm/glm.hpp>
#include <memory>

namespace StrikeEngine {

    class Skybox {
    public:
        Skybox();
        ~Skybox();

        void bind() const;
        void unbind() const;

        std::shared_ptr<CubeMap> getCubeMap() const { return mCubeMap; }
        std::shared_ptr<Shader> getShader() const { return mShader; }

        void setCubeMap(std::shared_ptr<CubeMap> cubeMap) { mCubeMap = cubeMap; }
        void setShader(std::shared_ptr<Shader> shader) { mShader = shader; }

    private:
        void setupCube();

        std::shared_ptr<CubeMap> mCubeMap;
        std::shared_ptr<Shader> mShader;
        GLuint mVAO;
        GLuint mVBO;
    };
}
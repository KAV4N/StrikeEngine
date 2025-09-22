#pragma once

#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Types/Shader.h"

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

        std::shared_ptr<CubeMap> getCubeMap() const { return mCubeMap; }
        std::shared_ptr<Shader> getShader() const { return mShader; }

        std::string getShaderId() const { return mShaderId; }
        std::string getCubeMapId() const { return mCubeMapId; }

        void setCubeMap(const std::string& cubeMapId);
        void setShader(const std::string& shaderId);

        bool isCubeMapValid() const { return mCubeMap && mCubeMap->isReady(); }
        bool isShaderValid() const { return mShader && mShader->isReady(); }

    private:
        void setupCube();

        std::string mShaderId;
        std::string mCubeMapId;

        std::shared_ptr<CubeMap> mCubeMap;
        std::shared_ptr<Shader> mShader;
        GLuint mVAO;
        GLuint mVBO;
        GLuint mEBO;
    };
}
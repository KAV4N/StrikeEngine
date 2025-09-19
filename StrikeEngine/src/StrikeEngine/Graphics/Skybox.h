#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#define SKYBOX_SHADER "SkyboxShader"
#define SKYBOX_TEXTURES_PATH "assets/textures/skybox/"

namespace StrikeEngine {

    class Shader;

    class Skybox {
    public:
        Skybox(
            const std::vector<std::string>& faces = {
                SKYBOX_TEXTURES_PATH "right.png", SKYBOX_TEXTURES_PATH "left.png",
                SKYBOX_TEXTURES_PATH "top.png", SKYBOX_TEXTURES_PATH "bottom.png",
                SKYBOX_TEXTURES_PATH "front.png", SKYBOX_TEXTURES_PATH "back.png"
            }
        );
        ~Skybox();

        void bind();
        void unbind();
        inline GLuint getTextureId() const { return mTextureId; }
        inline GLuint getVao() const { return mVao; }
        inline Shader* getShader() const { return mSkyboxShader; }
        void draw();

    private:
        void setupShader();
        void loadSkybox();
        void setupSkybox();

        std::vector<std::string> mFaces;
        GLuint mTextureId;
        GLuint mVao, mVbo, mEbo;
        Shader* mSkyboxShader;
        GLuint mNumIndices;
    };

}

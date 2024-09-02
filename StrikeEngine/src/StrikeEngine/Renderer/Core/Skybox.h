#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <StrikeEngine/Renderer/Core/Shader.h>

#define SKYBOX_SHADER "SkyboxShader"
#define SKYBOX_TEXTURES_PATH "assets/textures/skybox/"

namespace StrikeEngine {

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

        void Bind();
        void Unbind();
        inline GLuint GetTextureID() const { return m_TextureID; }
        inline GLuint GetVAO() const { return m_VAO; }
        inline Shader* GetShader() const { return m_SkyboxShader; }
        void Draw();

    private:
        void SetupShader();
        void LoadSkybox();
        void SetupSkybox();

        std::vector<std::string> m_Faces;
        GLuint m_TextureID;
        GLuint m_VAO, m_VBO, m_EBO;
        Shader* m_SkyboxShader;
        GLuint m_NumIndices;
    };

}

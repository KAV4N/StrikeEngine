#include "strikepch.h"
#include "Skybox.h"

#include <stb_image.h>
#include <iostream>
#include "StrikeEngine/Graphics/Managers/ShaderManager.h"

namespace StrikeEngine {

    Skybox::Skybox(const std::vector<std::string>& faces)
        : m_Faces(faces)
    {
        LoadSkybox();
        SetupSkybox();
        SetupShader();
    }

    Skybox::~Skybox() {
        glDeleteTextures(1, &m_TextureID);
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    void Skybox::SetupShader()
    {
        m_SkyboxShader = ShaderManager::Get()->GetShader(SKYBOX_SHADER);
    }

    void Skybox::LoadSkybox() {
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

        //stbi_set_flip_vertically_on_load(true);
        int width, height, channels;
        for (GLuint i = 0; i < m_Faces.size(); i++) {
            stbi_uc* data = stbi_load(m_Faces[i].c_str(), &width, &height, &channels, 0);
            if (!data) {
                std::cerr << "Failed to load texture: " << m_Faces[i] << std::endl;
                continue;
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Skybox::SetupSkybox() {
        float skyboxVertices[] =
        {
            //   Coordinates
            -1.0f, -1.0f,  1.0f, // 0
             1.0f, -1.0f,  1.0f, // 1
             1.0f, -1.0f, -1.0f, // 2
            -1.0f, -1.0f, -1.0f, // 3
            -1.0f,  1.0f,  1.0f, // 4
             1.0f,  1.0f,  1.0f, // 5
             1.0f,  1.0f, -1.0f, // 6
            -1.0f,  1.0f, -1.0f  // 7
        };

        unsigned int indices[] =
        {
            // Front face (towards the inside of the cube)
            3, 2, 6,
            6, 7, 3,
            // Back face
            0, 4, 5,
            5, 1, 0,
            // Left face
            0, 3, 7,
            7, 4, 0,
            // Right face
            1, 5, 6,
            6, 2, 1,
            // Top face
            4, 7, 6,
            6, 5, 4,
            // Bottom face
            0, 1, 2,
            2, 3, 0
        };

        m_NumIndices = sizeof(indices) / sizeof(indices[0]);

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }



    void Skybox::Draw() {
        glBindVertexArray(m_VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
        glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Skybox::Bind() {
        glBindVertexArray(m_VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
    }

    void Skybox::Unbind() {
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

}

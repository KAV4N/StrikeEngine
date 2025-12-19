#include "strikepch.h"
#include "FontRenderer.h"
#include "StrikeEngine/Core/Application.h"

#include "StrikeEngine/Graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace StrikeEngine {

    FontRenderer& FontRenderer::get() {
        static FontRenderer instance;
        return instance;
    }

    FontRenderer::~FontRenderer() {
        cleanup();
    }

    void FontRenderer::init() {
        if (mInitialized) return;

        // Initialize FreeType
        if (FT_Init_FreeType(&mFT)) {
            std::cerr << "Failed to initialize FreeType" << std::endl;
            return;
        }


        loadFont("fonts/ARIAL.ttf", 48);

        // Get shader
        mShader = ShaderManager::get().getShader("text.glsl");
        if (!mShader) {
            std::cerr << "Failed to load text shader" << std::endl;
            return;
        }

        // Setup VAO/VBO
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mInitialized = true;
    }

    void FontRenderer::loadFont(const std::string& fontPath, unsigned int fontSize) {
        if (FT_New_Face(mFT, fontPath.c_str(), 0, &mFace)) {
            std::cerr << "Failed to load font: " << fontPath << std::endl;
            return;
        }

        FT_Set_Pixel_Sizes(mFace, 0, fontSize);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load ASCII characters
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(mFace, c, FT_LOAD_RENDER)) {
                std::cerr << "Failed to load glyph: " << c << std::endl;
                continue;
            }

            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mFace->glyph->bitmap.width,
                        mFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                        mFace->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
                glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
                static_cast<unsigned int>(mFace->glyph->advance.x)
            };
            mCharacters[c] = character;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void FontRenderer::renderText(const std::string& text, float x, float y, float fontScale, const glm::vec3& color) {
        if (!mInitialized) return;

        auto& window = Application::get().getWindow();
        float screenX = x;
        float screenY = y;
        float scale = fontScale; 

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mShader->bind();
        
        // Orthographic projection
        glm::mat4 projection = glm::ortho(0.0f, (float)window.getWidth(), 0.0f, (float)window.getHeight());
        mShader->setMat4("projection", projection);
        mShader->setVec3("textColor", color / 255.0f); // Convert from 0-255 to 0-1

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(mVAO);

        for (char c : text) {
            Character ch = mCharacters[c];

            float xpos = screenX + ch.bearing.x * scale;
            float ypos = screenY - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            glBindBuffer(GL_ARRAY_BUFFER, mVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            screenX += (ch.advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    }

    void FontRenderer::cleanup() {
        if (!mInitialized) return;

        for (auto& pair : mCharacters) {
            glDeleteTextures(1, &pair.second.textureID);
        }
        mCharacters.clear();

        if (mVAO) glDeleteVertexArrays(1, &mVAO);
        if (mVBO) glDeleteBuffers(1, &mVBO);

        if (mFace) FT_Done_Face(mFace);
        if (mFT) FT_Done_FreeType(mFT);

        mInitialized = false;
    }

}
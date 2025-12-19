#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ft2build.h>

#include FT_FREETYPE_H
#include <unordered_map>

#include <memory>
#include <string>

namespace StrikeEngine {

    class Shader;

    struct Character {
        GLuint textureID;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };

    class FontRenderer {
    public:
        static FontRenderer& get();

        void init();
        void cleanup();

        void renderText(const std::string& text, float x, float y, float fontScale, const glm::vec3& color);

    private:
        FontRenderer() = default;
        ~FontRenderer();
        FontRenderer(const FontRenderer&) = delete;
        FontRenderer& operator=(const FontRenderer&) = delete;
        FontRenderer(FontRenderer&&) = delete;
        FontRenderer& operator=(FontRenderer&&) = delete;

        void loadFont(const std::string& fontPath, unsigned int fontSize);

        std::unordered_map<char, Character> mCharacters;
        std::shared_ptr<Shader> mShader;
        GLuint mVAO = 0;
        GLuint mVBO = 0;
        FT_Library mFT;
        FT_Face mFace;
        bool mInitialized = false;
    };

}
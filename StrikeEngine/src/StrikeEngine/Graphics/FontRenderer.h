#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ft2build.h>

#include FT_FREETYPE_H
#include <unordered_map>

#include <memory>
#include <string>

namespace Strike {

    class Shader;

    /**
     * @brief Structure representing a character glyph for text rendering
     */
    struct Character {
        GLuint textureID;        /**< OpenGL texture ID for the character glyph */
        glm::ivec2 size;         /**< Size of the character glyph in pixels */
        glm::ivec2 bearing;      /**< Offset from baseline to top-left of glyph */
        unsigned int advance;    /**< Horizontal advance to next character */
    };

    /**
     * @brief Singleton class for rendering text using FreeType font rendering
     * 
     * Handles loading fonts, creating character glyphs, and rendering text
     * to the screen with various formatting options.
     * 
     * @note Uses instanced rendering for efficient text drawing
     */
    class FontRenderer {
    public:
        /**
         * @brief Get the singleton instance of FontRenderer
         * 
         * @return Reference to the FontRenderer instance
         */
        static FontRenderer& get();

        /**
         * @brief Initialize the font renderer system
         * 
         * Sets up FreeType library, loads default font, and creates
         * OpenGL buffers for rendering.
         */
        void init();

        /**
         * @brief Clean up font renderer resources
         * 
         * Frees all OpenGL resources and FreeType objects.
         */
        void cleanup();

        /**
         * @brief Render text to the screen
         * 
         * @param text The string to render
         * @param x The x-coordinate for text position
         * @param y The y-coordinate for text position
         * @param fontScale Scale factor for the font size
         * @param color RGB color of the text (0-255 range)
         * @param pivot Pivot point for positioning (0-1 range, default bottom-left)
         */
        void renderText(const std::string& text, float x, float y, float fontScale, 
                       const glm::vec3& color, const glm::vec2& pivot = glm::vec2(0.0f));

    private:
        FontRenderer() = default;
        ~FontRenderer();
        FontRenderer(const FontRenderer&) = delete;
        FontRenderer& operator=(const FontRenderer&) = delete;
        FontRenderer(FontRenderer&&) = delete;
        FontRenderer& operator=(FontRenderer&&) = delete;

        void loadFont(const std::string& fontPath, unsigned int fontSize);
        glm::vec2 calculateTextSize(const std::string& text, float fontScale);

        std::unordered_map<char, Character> mCharacters;
        std::shared_ptr<Shader> mShader;
        GLuint mVAO = 0;
        GLuint mVBO = 0;
        FT_Library mFT;
        FT_Face mFace;
        bool mInitialized = false;
    };
}
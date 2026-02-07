#pragma once

#include "StrikeEngine/Core/Core.h"
#include <utility>

namespace StrikeEngine {

    /**
     * @brief Cursor display modes.
     */
    enum class CursorMode {
        Normal,  ///< Normal visible cursor
        Hidden,  ///< Hidden cursor
        Locked   ///< Cursor locked and hidden
    };

    /**
     * @brief Input handling class for keyboard and mouse input.
     */
    class Input {
    public:
        /**
         * @brief Check if a key is currently pressed.
         * @param keyCode The key code to check.
         * @return True if the key is pressed, false otherwise.
         */
        static bool isKeyPressed(int keyCode);

        /**
         * @brief Check if a mouse button is currently pressed.
         * @param button The mouse button code to check.
         * @return True if the mouse button is pressed, false otherwise.
         */
        static bool isMouseButtonPressed(int button);

        /**
         * @brief Get the current mouse X position.
         * @return Mouse X position in pixels.
         */
        static float getMouseX();
        
        /**
         * @brief Get the current mouse Y position.
         * @return Mouse Y position in pixels.
         */
        static float getMouseY();

        /**
         * @brief Get the current mouse X and Y positions.
         * @return Pair of mouse X and Y positions in pixels.
         */
        static std::pair<float, float> getMouseXY();
        
        /**
         * @brief Set the cursor mode.
         * @param mode The cursor mode to set.
         */
        static void setCursorMode(CursorMode mode);
    };
}
#pragma once

#include "StrikeEngine/Core/Core.h"
#include <utility>

namespace Strike {

    /**
     * @brief Cursor display modes.
     */
    enum class CursorMode {
        Normal,  ///< Normal visible cursor
        Hidden,  ///< Hidden cursor
        Locked   ///< Cursor locked and hidden, raw input enabled if supported
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
         * @brief Get the mouse movement delta since the last call to update().
         * @note Use this for camera rotation instead of getMouseXY() when the
         *       cursor is locked. Returns (0, 0) if the mouse did not move.
         * @return Pair of delta X and delta Y in pixels.
         */
        static std::pair<float, float> getMouseDelta();

        /**
         * @brief Update input state. called in application
         */
        static void update();

        /**
         * @brief Set the cursor mode.
         * @param mode The cursor mode to set.
         * @note Setting Locked will also enable raw mouse motion if the
         *       platform supports it, eliminating OS acceleration.
         *       Resetting to Normal or Hidden will disable raw mouse motion.
         */
        static void setCursorMode(CursorMode mode);

    private:
        static float sLastX;
        static float sLastY;
        static float sDeltaX;
        static float sDeltaY;
    };
}
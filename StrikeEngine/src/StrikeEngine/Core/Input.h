#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

    /**
     * @brief Input handling class use for input handling.
     */
    class Input {
    public:
        /**
         * @brief Check if a key is currently pressed.
         * @param keyCode The key code to check.
         * @return True if the key is pressed, false otherwise.
         */
        inline static bool isKeyPressed(int keyCode) { return sInstance->isKeyPressedImpl(keyCode); }

        /**
         * @brief Check if a mouse button is currently pressed.
         * @param button The mouse button code to check.
         * @return True if the mouse button is pressed, false otherwise.
         */
        inline static bool isMouseButtonPressed(int button) { return sInstance->isMouseButtonPressedImpl(button); }

        /**
         * @brief Get the current mouse X position.
         * @return Mouse X position.
         */
        inline static float getMouseX() { return sInstance->getMouseXImpl(); }
        
        /**
         * @brief Get the current mouse Y position.
         * @return Mouse Y position.
         */
        inline static float getMouseY() { return sInstance->getMouseYImpl(); }

        /**
         * @brief Get the current mouse X and Y positions.
         * @return Pair of mouse X and Y positions.
         */
        inline static std::pair<float, float> getMouseXY() { return sInstance->getMouseXYImpl(); }
        
        /**
         * @brief Set the cursor mode.
         * @param mode The cursor mode to set.
         */
        inline static void setCursorMode(CursorMode mode) { sInstance->setCursorModeImpl(mode); }

    protected:
        virtual bool isKeyPressedImpl(int keyCode) = 0;
        virtual bool isMouseButtonPressedImpl(int keyCode) = 0;
        virtual float getMouseXImpl() = 0;
        virtual float getMouseYImpl() = 0;
        virtual std::pair<float, float> getMouseXYImpl() = 0;
        virtual void setCursorModeImpl(CursorMode mode) = 0;

    private:
        static Input* sInstance;
    };
}
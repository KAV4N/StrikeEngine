#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

    class Input {
    public:
        inline static bool isKeyPressed(int keyCode) { return sInstance->isKeyPressedImpl(keyCode); }
        inline static bool isMouseButtonPressed(int button) { return sInstance->isMouseButtonPressedImpl(button); }
        inline static float getMouseX() { return sInstance->getMouseXImpl(); }
        inline static float getMouseY() { return sInstance->getMouseYImpl(); }
        inline static std::pair<float, float> getMouseXY() { return sInstance->getMouseXYImpl(); }
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
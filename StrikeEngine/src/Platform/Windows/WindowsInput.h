#pragma once

#include "StrikeEngine/Core/Input.h"

namespace StrikeEngine {

    class WindowsInput : public Input {
    protected:
        bool isKeyPressedImpl(int keycode) override;
        bool isMouseButtonPressedImpl(int keyCode) override;
        float getMouseXImpl() override;
        float getMouseYImpl() override;
        std::pair<float, float> getMouseXYImpl() override;
        void setCursorModeImpl(CursorMode mode) override;
    };
}
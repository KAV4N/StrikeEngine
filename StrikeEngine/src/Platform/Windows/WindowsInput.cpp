#include "strikepch.h"
#include "WindowsInput.h"
#include "StrikeEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace StrikeEngine {

    Input* Input::sInstance = new WindowsInput();

    bool WindowsInput::isKeyPressedImpl(int keycode) {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindowsInput::isMouseButtonPressedImpl(int button) {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    float WindowsInput::getMouseXImpl() {
        return getMouseXYImpl().first;
    }

    float WindowsInput::getMouseYImpl() {
        return getMouseXYImpl().second;
    }

    std::pair<float, float> WindowsInput::getMouseXYImpl() {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return std::make_pair(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void WindowsInput::setCursorModeImpl(CursorMode mode) {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        switch (mode) {
        case CursorMode::Normal:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case CursorMode::Locked:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        case CursorMode::Hidden:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        }
    }
}
#include "strikepch.h"
#include "Input.h"
#include "StrikeEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Strike {

    bool Input::isKeyPressed(int keycode) {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::isMouseButtonPressed(int button) {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    float Input::getMouseX() {
        return getMouseXY().first;
    }

    float Input::getMouseY() {
        return getMouseXY().second;
    }

    std::pair<float, float> Input::getMouseXY() {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return std::make_pair(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void Input::setCursorMode(CursorMode mode) {
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
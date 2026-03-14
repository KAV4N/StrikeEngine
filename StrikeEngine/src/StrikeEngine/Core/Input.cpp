#include "strikepch.h"
#include "Input.h"
#include "StrikeEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Strike {

    float Input::sLastX  = 0.0f;
    float Input::sLastY  = 0.0f;
    float Input::sDeltaX = 0.0f;
    float Input::sDeltaY = 0.0f;

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

    float Input::getMouseX() { return getMouseXY().first; }
    float Input::getMouseY() { return getMouseXY().second; }

    std::pair<float, float> Input::getMouseXY() {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }

    std::pair<float, float> Input::getMouseDelta() {
        return { sDeltaX, sDeltaY };
    }

    void Input::update() {
        auto [x, y] = getMouseXY();
        sDeltaX = x - sLastX;
        sDeltaY = y - sLastY;
        sLastX  = x;
        sLastY  = y;
    }

    void Input::setCursorMode(CursorMode mode) {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

        switch (mode) {
            case CursorMode::Normal:
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;

            case CursorMode::Hidden:
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;

            case CursorMode::Locked:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                if (glfwRawMouseMotionSupported())
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                auto [x, y] = getMouseXY();
                sLastX = x;
                sLastY = y;
                break;
        }
    }
}
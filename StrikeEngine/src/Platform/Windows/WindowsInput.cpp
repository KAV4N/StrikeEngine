#include "strikepch.h"
#include "WindowsInput.h"

#include "StrikeEngine/Core/Application.h"
#include <GLFW/glfw3.h>



namespace StrikeEngine {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	float WindowsInput::GetMouseXImpl()
	{
		return GetMouseXYImpl().first;
	}

	float WindowsInput::GetMouseYImpl()
	{
		return GetMouseXYImpl().second;
	}
	std::pair<float,float> WindowsInput::GetMouseXYImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return std::make_pair(static_cast<float>(xpos), static_cast<float>(ypos));
	}

	void WindowsInput::SetCursorModeImpl(CursorMode mode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
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
#include "strikepch.h"
#include "OpenGLContext.h"



namespace StrikeEngine
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		STRIKE_CORE_ASSERT(windowHandle, "WindowHandle is null!!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		STRIKE_CORE_ASSERT(status, "Failed to init Glad!");


	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
#pragma once

#include "StrikeEngine/Renderer/RendererContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

class GLFWwindow;

namespace StrikeEngine
{
	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};

}

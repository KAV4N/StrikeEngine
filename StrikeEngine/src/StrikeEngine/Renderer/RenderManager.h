#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>
#include <StrikeEngine/Entity/Model.h>

class GLFWwindow;

namespace StrikeEngine
{
	class RenderManager
	{
	public:
		RenderManager(GLFWwindow* windowHandle);
		~RenderManager();
		virtual void Init();
		virtual void SwapBuffers();
		virtual void Render(Model* model);
		virtual void Clear();
		virtual void CleanUp();
		
	private:
		GLFWwindow* m_WindowHandle;
	};

}

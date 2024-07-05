#include "strikepch.h"
#include "RenderManager.h"


namespace StrikeEngine
{
	RenderManager::RenderManager(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		STRIKE_CORE_ASSERT(windowHandle, "WindowHandle is null!!");
	}

	RenderManager::~RenderManager()
	{
	}


	void RenderManager::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		STRIKE_CORE_ASSERT(status, "Failed to init Glad!");


	}
	void RenderManager::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void RenderManager::Render(Model* model)
	{
		Clear();
		glBindVertexArray(model->GetId());
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, model->GetVertexCount());
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}


	void RenderManager::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void RenderManager::CleanUp()
	{

	}
}
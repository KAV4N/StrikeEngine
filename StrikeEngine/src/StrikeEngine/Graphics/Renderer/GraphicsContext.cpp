#include "strikepch.h"
#include "StrikeEngine/Graphics/Renderer/GraphicsContext.h"

#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace StrikeEngine {

	std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    STRIKE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		STRIKE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
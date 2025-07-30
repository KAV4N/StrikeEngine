#pragma once

#include "StrikeEngine/Graphics/Renderer/RendererAPI.h"


namespace StrikeEngine {

	class Renderer
	{
	public:
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}
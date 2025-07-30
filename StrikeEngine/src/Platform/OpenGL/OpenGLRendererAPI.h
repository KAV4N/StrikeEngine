#pragma once

#include "StrikeEngine/Graphics/Renderer/RendererAPI.h"

namespace StrikeEngine {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		virtual void SetClearColor(const glm::vec4& color);
		virtual void Clear();


	};

}
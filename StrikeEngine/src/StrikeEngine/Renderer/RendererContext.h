#pragma once


namespace StrikeEngine {
	class RendererContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}
#pragma once

#include <glm/glm.hpp>


namespace StrikeEngine {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		static API GetAPI() { return s_API; }
		static std::unique_ptr <RendererAPI> Create();
	private:
		static API s_API;
	};

}
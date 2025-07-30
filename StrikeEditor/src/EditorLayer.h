#pragma once

#include "StrikeEngine.h"
#include <entt/entt.hpp>

namespace StrikeEngine {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(float deltaTime) override;
		virtual void OnImGuiRender() override;
		void OnRender() override;
		void OnEvent(Event& e) override;
	
	private:

		bool m_ViewportFocused = false, m_ViewportHovered = false;

	};

}
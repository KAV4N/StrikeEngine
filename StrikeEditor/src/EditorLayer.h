#pragma once

#include "StrikeEngine.h"
#include <entt/entt.hpp>
#include <StrikeEngine/Graphics/Core/CameraController.h>

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
		ScreenPanel* m_ViewportPanel;
		CameraController m_CameraController;
		bool m_ViewportFocused = false, m_ViewportHovered = false;

	};

}
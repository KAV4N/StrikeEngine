#pragma once

#include "StrikeEngine/Core/Layer.h"

#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/Events/KeyEvent.h"
#include "StrikeEngine/Events/MouseEvent.h"

namespace StrikeEngine {
	class STRIKE_API ImGuiLayer : public Layer 
	{
	public: 
		ImGuiLayer();
		~ImGuiLayer();


		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();


		void SetupImGuiStyle();
	private:
		float m_Time = 0.0f;
	};
}
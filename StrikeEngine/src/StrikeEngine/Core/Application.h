#pragma once

#include "Core.h"

#include "Window.h"
#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"

#include "StrikeEngine/ImGui/ImGuiLayer.h"

namespace StrikeEngine 
{


	class STRIKE_API Application 
	{
	
	public: 
		Application();

		virtual ~Application();

		void OnEvent(Event& e);

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }

		inline Window& GetWindow(){ return *m_Window; }
	
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		ImGuiLayer* m_ImGuiLayer;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;

	};

	Application* CreateApplication();
}




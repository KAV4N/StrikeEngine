#include "strikepch.h"
#include "Application.h"

#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Renderer/RenderManager.h"
#include "StrikeEngine/Renderer/ObjectLoader.h"

#include "Input.h"

#include <glad/glad.h>


#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace StrikeEngine 
{

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		STRIKE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		RenderManager::Create();
		RenderManager::Get().Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}


	Application::~Application(){}


	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		//STRIKE_CORE_TRACE("{0}", e);

		auto it = m_LayerStack.end();
		while (it != m_LayerStack.begin())
		{
			--it;
			(*it)->OnEvent(e);
			if (e.Handled)
			{
				break;
			}
		}


	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}


	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run() 
	{
		// TEST RENDER
		float vertices[] = {
				-0.5f, 0.5f, 0.f,
				-0.5f, -0.5f, 0.f,
				0.5f, -0.5f, 0.f,
				0.5f, -0.5f, 0.f,
				0.5f, 0.5f, 0.f,
				-0.5f, 0.5f, 0.f
		};

		ObjectLoader* objectLoader = new ObjectLoader();
		Model* model = objectLoader->LoadModel(vertices, 18);


		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		while (m_Running)
		{
			glClearColor(0.20f, 0.25f, 0.29f, 1);
			glClear(GL_COLOR_BUFFER_BIT);	
			
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) 
			{
				layer->OnUpdate();
				layer->OnImGuiRender();
			}

			RenderManager::Render(model);

			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		}
	}
}

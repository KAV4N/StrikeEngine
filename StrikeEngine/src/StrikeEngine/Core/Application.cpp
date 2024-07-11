#include "strikepch.h"
#include "Application.h"

#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include "StrikeEngine/Renderer/ObjectLoader.h"
#include "StrikeEngine/Renderer/ShaderManager.h"

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

		
		Renderer::Create();
		Renderer::Get().Init();
		

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
		-0.5f,  0.5f, 0.0f,  // Top Left
		-0.5f, -0.5f, 0.0f,  // Bottom Left
		 0.5f, -0.5f, 0.0f,  // Bottom Right
		 0.5f,  0.5f, 0.0f   // Top Right
		};

		int indices[] = {
			0, 1, 2,   // First triangle
			2, 3, 0    // Second triangle
		};


		size_t indicesCount = sizeof(indices) / sizeof(indices[0]);
		size_t verticesCount = sizeof(vertices) / sizeof(vertices[0]);

		ObjectLoader* objectLoader = new ObjectLoader();
		RawModel* model = objectLoader->LoadModel(vertices, verticesCount, indices, indicesCount);
		


		ShaderManager::Get()->LoadShaderFromFile("DefaultVertexShader", "assets/shaders/VertexShader.glsl",GL_VERTEX_SHADER);
		ShaderManager::Get()->LoadShaderFromFile("DefaultFragmentShader", "assets/shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		ShaderManager::Get()->Bind();


		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		while (m_Running)
		{
			glClearColor(0.20f, 0.25f, 0.29f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
				layer->OnImGuiRender();
			}

			Renderer::Render(model);

			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		}

	}
}

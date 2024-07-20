
#include "strikepch.h"
#include "Application.h"

#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include "StrikeEngine/Renderer/ModelManager.h"
#include "StrikeEngine/Renderer/ShaderManager.h"

#include "Input.h"
#include <glad/glad.h>
#include <StrikeEngine/Scene/Scene.h>
#include <StrikeEngine/Scene/World.h>


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
        Renderer::Init();
        Renderer::Get()->SetDefaultTexture("assets/textures/default.png");

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        m_World = new World();
        Scene* scene = new Scene();

        // Load models
        Model* model = ModelManager::Get()->LoadModel("assets/objects/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj");
        Entity* entity = new Entity(model);
        entity->IncreaseRotation(glm::uvec3(270.f, 0.f, 0.f));

        Model* model2 = ModelManager::Get()->LoadModel("assets/objects/penguin/PenguinBaseMesh.obj");
        model2->GetParts()[0]->AddTexture(new Texture("assets/objects/penguin/Penguin Diffuse Color.png"));
        Entity* entity2 = new Entity(model2);
        entity2->SetPosition(glm::vec3(5.f, 0.f, 0.f));

        scene->AddEntity(entity);
        scene->AddEntity(entity2);

        Light* light1 = new Light(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 3.f);
        Light* light2 = new Light(glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),2.5f);
        scene->AddLight(light1);
        scene->AddLight(light2);

        m_World->AddScene(scene);
    }


    Application::~Application()
    {
        delete m_World;
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::Run() {
        ShaderManager::Get()->LoadShader("default", "assets/shaders/InlineShader.glsl");
        Shader* shader = ShaderManager::Get()->GetShader("default");

        while (m_Running) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer::Get()->BeginScene(m_World->GetActiveScene()->GetCamera());

            m_World->Update();
            m_World->Render(shader);

            Renderer::Get()->EndScene();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }


    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
}
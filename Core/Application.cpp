
#include "strikepch.h"
#include "Application.h"

#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include "StrikeEngine/Renderer/ObjectLoader.h"
#include "StrikeEngine/Renderer/ShaderManager.h"
#include "StrikeEngine/Renderer/TextureManager.h"

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

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        m_World = new World();
        Scene* scene = new Scene();

        float vertices[] = {
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,

            -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            0.5f, 0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,

            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,

            -0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f
        };


        float textureCoords[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };


        int indices[] = {
           0, 1, 3,
           3, 1, 2,
           4, 5, 7,
           7, 5, 6,
           8, 9, 11,
           11, 9, 10,
           12, 13, 15,
           15, 13, 14,
           16, 17, 19,
           19, 17, 18,
           20, 21, 23,
           23, 21, 22
        };



        size_t indicesCount = sizeof(indices) / sizeof(indices[0]);
        size_t verticesCount = sizeof(vertices) / sizeof(vertices[0]);
        size_t textureCount = sizeof(textureCoords) / sizeof(textureCoords[0]);

        TextureManager::Get()->LoadTexture("test", "assets/textures/untitled.png");
        Texture* texture = TextureManager::Get()->GetTexture("test");

        ObjectLoader* objectLoader = new ObjectLoader();
        Model* model = objectLoader->LoadModel(vertices, verticesCount, indices, indicesCount, texture, textureCoords, textureCount);
        Entity* entity = new Entity(model);

        scene->AddEntity(entity);

        Camera* camera = new Camera(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
        scene->SetCamera(camera);

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
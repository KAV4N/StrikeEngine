#include "strikepch.h"
#include "Application.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "Input.h"
#include <glad/glad.h>
#include <chrono>


#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace StrikeEngine {
    Application* Application::sInstance = nullptr;

    Application::Application() {
        STRIKE_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        mWindow = std::unique_ptr<Window>(Window::create());
        mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));

        createManagers();

        // Store reference to World singleton
        mWorld = &World::get();

        mImGuiLayer = new ImGuiLayer();
        mImGuiLayer->onAttach();

        // Initialize timer with default values
        mTimer.setTargetFPS(60.0f);
    }

    Application::~Application() {
        if (mImGuiLayer) {
            delete mImGuiLayer;
        }
        // Note: mWorld is not deleted as it's a singleton reference
    }

    void Application::createManagers() {
    }

    void Application::onEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

        // Let the World handle events
        mWorld->onEvent(e);
    }

    void Application::onUpdate() {


        mTimer.updateFrame();
        float deltaTime = mTimer.getDeltaTime();

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        AssetManager::get().update();

        // Update world
        mWorld->update(deltaTime);

        // Render the World and imgui
        mImGuiLayer->begin();
        mWorld->onRender();
        Renderer::get().display();
        mWorld->onImGuiRender();
        mImGuiLayer->end();

        mWindow->onUpdate();      
        // Limit frame rate
        mTimer.limitFrameRate();
    }

    void Application::run() {
        mTimer.reset();

        while (mRunning) {
            onUpdate();
        }
    }

    bool Application::onWindowClose(WindowCloseEvent& e) {
        mRunning = false;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& e) {
        
        Renderer::get().resize(e.getWidth(), e.getHeight());
        return true;
    }
}
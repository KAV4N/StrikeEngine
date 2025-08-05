#include "strikepch.h"
#include "Application.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "Input.h"
#include <glad/glad.h>
#include <chrono>
#include <imgui.h>

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace StrikeEngine {
    Application* Application::sInstance = nullptr;

    Application::Application() {
        STRIKE_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;
        mWindow = std::unique_ptr<Window>(Window::create());
        mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));

        createManagers();

        mImGuiLayer = new ImGuiLayer();
        pushOverlay(mImGuiLayer);
    }

    Application::~Application() {
    }

    void Application::createManagers() {
    }

    void Application::pushLayer(Layer* layer) {
        mLayerStack.pushLayer(layer);
        layer->onAttach();
    }

    void Application::pushOverlay(Layer* overlay) {
        mLayerStack.pushOverlay(overlay);
        overlay->onAttach();
    }

    void Application::onEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

        for (auto it = mLayerStack.end(); it != mLayerStack.begin();) {
            (*--it)->onEvent(e);
            if (e.handled)
                break;
        }
    }

    void Application::onUpdate() {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = now - mLastFrameTime;
        mLastFrameTime = now;
        float deltaTime = deltaTimeDuration.count();

        AssetManager::get().update();

        for (Layer* layer : mLayerStack)
            layer->onUpdate(deltaTime);

        mImGuiLayer->begin();
        for (Layer* layer : mLayerStack) {
            layer->onRender();
            layer->onImGuiRender();
        }
        mImGuiLayer->end();

        mWindow->onUpdate();
    }

    void Application::run() {
        mLastFrameTime = std::chrono::high_resolution_clock::now();
        while (mRunning) {
            onUpdate();
        }
    }

    bool Application::onWindowClose(WindowCloseEvent& e) {
        mRunning = false;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& e) {
        return true;
    }
}
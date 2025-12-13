#include "strikepch.h"
#include "Application.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "Input.h"
#include <glad/glad.h>
#include <chrono>
#include <thread>

#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"

#include "Profiler.h"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace StrikeEngine {
    
    Application* Application::sInstance = nullptr;

    Application::Application() {
        STRIKE_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        mWindow = std::unique_ptr<Window>(Window::create());
        mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));

        Renderer& renderer = Renderer::get();
        renderer.init();
        renderer.resize(mWindow.get()->getWidth(), mWindow.get()->getHeight());
        mWindow->setVSync(false);
    }

    Application::~Application() {

    }

    void Application::onEvent(Event& e) {
        if (mRunning){
            EventDispatcher dispatcher(e);

            dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
            dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));
            dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(Application::printProfiler));
            World::get().onEvent(e);
        }
    }

    bool Application::printProfiler(KeyPressedEvent& e){
        Profiler::printResults();
        return true;
    }

    void Application::onUpdate(float deltaTime) {
        World& world = World::get();
        world.onUpdate(deltaTime);
        world.onRender();
        
        Renderer::get().display();
        AssetManager::get().update();       

       
        mWindow->setWindowTitle("StrikeEngine - FPS: " + std::to_string(static_cast<int>(mCurrentFPS)));
    
        mWindow->onUpdate();
    }

    void Application::run() {
        PROFILE_SCOPE("AppRun");

        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (mRunning) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - lastFrameTime;
            double deltaTime = elapsed.count();
            mCurrentFPS = 1.0 / deltaTime;
            
            if (mTargetFPS <= 0) {
                onUpdate(static_cast<float>(deltaTime));
                lastFrameTime = currentTime;
            } else {
                const double targetFrameTime = 1.0 / mTargetFPS;
                
                if (deltaTime >= targetFrameTime) {
                    onUpdate(static_cast<float>(deltaTime));
                    lastFrameTime = currentTime;
                }
            }
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
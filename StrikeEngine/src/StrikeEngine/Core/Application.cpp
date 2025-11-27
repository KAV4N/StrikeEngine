#include "strikepch.h"
#include "Application.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "Input.h"
#include <glad/glad.h>
#include <chrono>

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

        // Initialize timer with default values
        //mTimer.setTargetFPS(60.0f);
    }

    Application::~Application() {

    }


    void Application::onEvent(Event& e) {
        EventDispatcher dispatcher(e);

        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));
        dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(Application::printProfiler));
        // Let the World handle events
        World::get().onEvent(e);
    }

    bool Application::printProfiler(KeyPressedEvent& e){
        Profiler::printResults();
        return true;
    }

    void Application::onUpdate() {


        mTimer.updateFrame();
        float deltaTime = mTimer.getDeltaTime();

       
        World& world = World::get();

        world.onUpdate(deltaTime);
        
        world.onRender();
        Renderer::get().display();

        AssetManager::get().update();   

        mWindow->onUpdate();      

        std::stringstream titleStream;
        titleStream << mTimer.getCurrentFPS();

        mWindow->setWindowTitle(titleStream.str());
        // Limit frame rate
        //mTimer.limitFrameRate();
    }

    void Application::run() {
        PROFILE_SCOPE("AppRUn");
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
        STRIKE_CORE_INFO("RESIZEDASSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS");
        return true;
    }
}
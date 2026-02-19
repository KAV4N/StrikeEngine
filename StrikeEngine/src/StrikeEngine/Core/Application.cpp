#include "strikepch.h"
#include "Application.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "Input.h"

#include <glad/glad.h>

#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Graphics/FontRenderer.h"

#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Systems/AudioSystem.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"

#include <chrono>

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace StrikeEngine {
    
    Application* Application::sInstance = nullptr;

    Application::Application() {
        STRIKE_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        mWindow = std::make_unique<Window>();
        mWindow->setEventCallback(BIND_EVENT_FN(Application::onEvent));

        Renderer& renderer = Renderer::get();
        renderer.init();
        renderer.resize(mWindow->getWidth(), mWindow->getHeight());

        FontRenderer::get().init();

        mWindow->setVSync(false);
    }

    Application::~Application() {
    }

    void Application::onEvent(Event& e) {
        if (mRunning) {
            EventDispatcher dispatcher(e);
            dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
            dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));
            World::get().onEvent(e);
        }
    }

    void Application::onUpdate(float deltaTime) {
        World& world = World::get();
        world.onUpdate(deltaTime);
        AssetManager::get().update();   
        world.onRender();
        mWindow->onUpdate();
    }

    void Application::run() {
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

        AssetManager::get().shutdown();
        World::get().shutDownSystems();

        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& e) {
        Renderer::get().resize(e.getWidth(), e.getHeight());
        return true;
    }

    float Application::getMasterVolume() const{
        return World::get().mAudioSystem->getMasterVolume();
    }

    void Application::setMasterVolume(float volume){
        World::get().mAudioSystem->setMasterVolume(volume);
    }


}
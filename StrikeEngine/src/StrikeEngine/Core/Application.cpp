#include "strikepch.h"
#include "Application.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "Input.h"

#include <glad/glad.h>

#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Graphics/FontRenderer.h"

#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Systems/AudioSystem.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"

#include <chrono>

namespace Strike {
    
    Application* Application::sInstance = nullptr;

    Application::Application() {
        STRIKE_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        Strike::Log::init();

        mWindow = std::make_unique<Window>();
        mWindow->setEventCallback([this](Event& e) { onEvent(e); });

        Renderer& renderer = Renderer::get();
        renderer.init();
        renderer.resize(mWindow->getWidth(), mWindow->getHeight());

        FontRenderer::get().init();

        mWindow->setVSync(false);
    }

    Application::~Application() {
    }

    void Application::onEvent(Event& e) {
        if (!mRunning)
            return;

        if (e.getEventType() == EventType::WindowClose) {
            onWindowClose(static_cast<WindowCloseEvent&>(e));
        } else if (e.getEventType() == EventType::WindowResize) {
            onWindowResize(static_cast<WindowResizeEvent&>(e));
        }

        if (!e.handled) {
            World::get().onEvent(e);
        }
    }

    void Application::onUpdate(float deltaTime) {
        Input::update();
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

        e.handled = true;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& e) {
        Renderer::get().resize(e.getWidth(), e.getHeight());
        e.handled = true;
        return true;
    }

    float Application::getMasterVolume() const {
        STRIKE_CORE_ASSERT(sInstance, "Application instance not initialized");
        return World::get().mAudioSystem->getMasterVolume();
    }

    void Application::setMasterVolume(float volume) {
        STRIKE_CORE_ASSERT(sInstance, "Application instance not initialized");
        World::get().mAudioSystem->setMasterVolume(volume);
    }

    float Application::getAudioAmplitude(const Entity& entity) const {
        STRIKE_CORE_ASSERT(entity.isValid(), "getAudioAmplitude called on invalid Entity");
        STRIKE_CORE_ASSERT(sInstance, "Application instance not initialized");
        return World::get().mAudioSystem->getAmplitude(entity.getHandle());
    }

}
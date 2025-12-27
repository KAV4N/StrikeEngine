#include "strikepch.h"
#include "World.h"

#include "Scene.h"
#include "SceneLoader.h"
#include "StrikeEngine/Events/Event.h"
#include "Systems/RenderSystem.h"
#include "Systems/ScriptSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/AudioSystem.h"


#include "StrikeEngine/Graphics/Skybox.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"

#include <chrono>
#include <thread>

#include "StrikeEngine/Core/Profiler.h"

namespace StrikeEngine {


    World& World::get()
    {
        static World instance;
        return instance;
    }

    World::World()
        : mSceneLoader(std::make_unique<SceneLoader>())
        , mRenderSystem(std::make_unique<RenderSystem>())
        , mScriptSystem(std::make_unique<ScriptSystem>())
        , mPhysicsSystem(std::make_unique<PhysicsSystem>())
        , mAudioSystem(std::make_unique<AudioSystem>())
    {
        mAudioSystem->initialize();
    }


    void World::loadScene(const std::filesystem::path& path)
    {
        mPendingScene = mSceneLoader->loadScene(path);
    }


    void World::loadSceneAsync(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Scene file does not exist: " << path << std::endl;
            return;
        }

        if (mSceneLoader->isLoading()) {
            std::cout << "Cannot load scene: another scene is currently being loaded";
            return;
        }

        mPendingScene = mSceneLoader->loadSceneAsync(path);

        if (mPendingScene.valid()) {
            std::cout << "Started async loading of scene from: " << path << std::endl;
        }
        else {
            std::cerr << "Failed to start async loading of scene from: " << path << std::endl;
        }
    }


    Scene* World::getScene() const
    {
        return mCurrentScene.get();
    }

    bool World::isLoading() const
    {
        return mPendingScene.valid() && 
               mPendingScene.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
    }



    void World::onUpdate(float dt)
    {
        PROFILE_SCOPE("world");
        mSceneLoader->update();
        checkAndSwitchScene();

        if (mCurrentScene) {

            {
                PROFILE_SCOPE("Scene::onUpdate");
                mCurrentScene->onUpdate(dt);
            }

            {
                PROFILE_SCOPE("ScriptSystem::onUpdate");
                mScriptSystem->onUpdate(dt);
            }

            {
                PROFILE_SCOPE("PhysicsSystem::onUpdate");
                mPhysicsSystem->onUpdate(dt);
            }

            {
                PROFILE_SCOPE("AudioSystem::onUpdate");
                mAudioSystem->onUpdate(dt);
            }

            {
                PROFILE_SCOPE("RenderSystem::onUpdate");
                mRenderSystem->onUpdate(dt);
            }
        }

    }

    RayHit World::rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const
    {       
        return mPhysicsSystem->rayCast(origin, direction, maxDistance);
    }

    std::vector<RayHit> World::rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const
    {
        return mPhysicsSystem->rayCastAll(origin, direction, maxDistance);
    }
    
    void World::setGravity(const glm::vec3& gravity)
    {
        mPhysicsSystem->setGravity(gravity);
    }

    glm::vec3 World::getGravity() const
    {
        return mPhysicsSystem->getGravity();
    }

    void World::onRender()
    {
        mRenderSystem->onRender();
    }

    void World::onEvent(Event& e)
    {
        if (mCurrentScene) {
            mScriptSystem->onEvent(e);
        }
    }

    void World::resize(uint32_t width, uint32_t height)
    {
        mRenderSystem->resize(width, height);
    }


    void World::checkAndSwitchScene()
    {
        if (!mPendingScene.valid())
            return;

        auto status = mPendingScene.wait_for(std::chrono::seconds(0));
        if (status != std::future_status::ready)
            return;

        try {
            auto newScene = mPendingScene.get();
            if (newScene) {
                clearPhysicsWorld();
                if (mCurrentScene)
                    mCurrentScene->shutdown();
                mCurrentScene = std::move(newScene);
                mCurrentScene->setPhysicsSystem(mPhysicsSystem.get());
                std::cout << "Successfully switched to new scene (sync or async)" << std::endl;
            }
            else {
                mCurrentScene = nullptr;
                std::cerr << "Scene loading returned null pointer" << std::endl;
            }
        }
        catch (const std::exception& e) {
            mCurrentScene = nullptr;
            std::cerr << "Exception during scene switching: " << e.what() << std::endl;
        }
        mPendingScene = std::future<std::unique_ptr<Scene>>();
    }


    void World::clearPhysicsWorld()
    {
        if (mPhysicsSystem && mCurrentScene) {
            auto& registry = mCurrentScene->getRegistry();
            auto view = registry.view<PhysicsComponent>();
            for (auto entity : view) {
                mPhysicsSystem->removePhysics(entity);
            }
        }
    }

} 
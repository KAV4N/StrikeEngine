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
        if (!std::filesystem::exists(path)) {
            STRIKE_CORE_ERROR("Scene file does not exist: {}", path.string());
            return;
        }
        mPendingScenePath = path;
        mSceneLoadPending = true;
        std::cout << "Scene queued for loading: " << path.string() << std::endl;
    }

    void World::processSceneLoad()
    {
        if (!mSceneLoadPending) {
            return;
        }
        try {
            auto newScene = mSceneLoader->loadScene(mPendingScenePath);
            if (newScene) {
                clearPhysicsWorld();
                if (mCurrentScene) {
                    mCurrentScene->shutdown();
                }
                mCurrentScene = std::move(newScene);
                mCurrentScene->setPhysicsSystem(mPhysicsSystem.get());
            }
            else {
                STRIKE_CORE_ERROR("Failed to load scene: {}", mPendingScenePath.string());
            }
        }
        catch (const std::exception& e) {
            STRIKE_CORE_ERROR("Exception while loading scene: {}", e.what());
        }

        mSceneLoadPending = false;
        mPendingScenePath.clear();
    }

    Scene* World::getScene() const
    {
        return mCurrentScene.get();
    }

    void World::onUpdate(float dt)
    {
        PROFILE_SCOPE("world");

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

        // Unity-style: process scene load at end of frame (after all updates)
        processSceneLoad();
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
#pragma once
#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Core/Layer.h"
#include "Entity.h"
#include "SceneGraph.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {
    class SceneGraph;
    class Entity;
    class Layer;

    class Scene final {
    public:
        Scene(const std::string& id, const std::filesystem::path& path, const std::string& name = "Untitled");
        ~Scene() = default;

        // Entity management
        Entity createEntity(const std::string& id, const std::string& name = "Untitled");
        void destroyEntity(Entity entity);
        bool isEntityValid(Entity entity) const;
        Entity getEntity(entt::entity handle);
        Entity getEntityByName(const std::string& name);
        Entity getEntityById(const std::string& id);
        Entity getRootEntity() const;
        bool isRootEntity(Entity entity) const;
        void reset();

        // Update methods
        void onUpdate(float dt);      
        void onRender();             
        void onImGuiRender();       
        void onEvent(Event& e);      

        // Scene state
        void setActive(bool active) { mActive = active; }
        bool isActive() const { return mActive; }

        // Scene assets management
        const std::unordered_map<std::string, std::shared_ptr<Asset>>& getSceneAssets() const { return mSceneAssets; }
        std::unordered_map<std::string, std::shared_ptr<Asset>>& getSceneAssets() { return mSceneAssets; }
        void clearSceneAssets() { mSceneAssets.clear(); }

    private:
        entt::registry& getRegistry() { return mRegistry; }
        SceneGraph& getSceneGraph() { return *mSceneGraph; }
        void createRootEntity(const std::string& id, const std::string& name = "");

    private:
        friend class Entity;
        friend class SceneLoader;

        entt::registry mRegistry;
        std::unique_ptr<SceneGraph> mSceneGraph;
        Entity mRootEntity;
        std::unordered_map<std::string, std::shared_ptr<Asset>> mSceneAssets;

        bool mActive = true;
    };
}
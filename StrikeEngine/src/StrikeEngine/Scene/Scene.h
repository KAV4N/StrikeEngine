#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include "StrikeEngine/Asset/Types/Asset.h"
#include "Entity.h"
#include "SceneGraph.h"

namespace StrikeEngine {

    class SceneGraph;
    class Entity;

    class Scene final {
    public:
        Scene(const std::string& id, const std::filesystem::path& path, const std::string& name = "Untitled");
        ~Scene() = default;

        Entity createEntity(const std::string& id, const std::string& name = "Untitled");
        void destroyEntity(Entity entity);
        bool isEntityValid(Entity entity) const;

        Entity getEntity(entt::entity handle);
        Entity getEntityByName(const std::string& name);
        Entity getEntityById(const std::string& id);

        Entity getRootEntity() const;
        bool isRootEntity(Entity entity) const;

        void reset();
        void update(float dt);

    private:
        entt::registry& getRegistry() { return mRegistry; }
        SceneGraph& getSceneGraph() { return *mSceneGraph; }

        void createRootEntity(const std::string& id, const std::string& name = "");

    private:
        friend class Entity;

        entt::registry mRegistry;
        std::unique_ptr<SceneGraph> mSceneGraph;
        Entity mRootEntity;

        std::unordered_map<std::string, std::shared_ptr<Asset>> mSceneAssets;
    };

}
#include "strikepch.h"
#include "Scene.h"
#include "Entity.h"
#include "World.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Scene/Components/LogicComponent.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"
#include "StrikeEngine/Scene/Components/AudioSourceComponent.h"

#include "StrikeEngine/Scene/Systems/PhysicsSystem.h"
#include "StrikeEngine/Scene/Systems/AudioSystem.h"

#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "Components/CameraComponent.h"
#include "StrikeEngine/Asset/AssetManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


namespace Strike {

    Scene::Scene(const std::filesystem::path& path, const std::string& tag)
        : mId(tag), mSun() {
        setupComponentProtection();
    }

    Scene::~Scene() {
    }

    void Scene::setupComponentProtection() {
        mRegistry.on_destroy<PhysicsComponent>().connect<&Scene::onPhysicsComponentDestroy>(this);
        mRegistry.on_destroy<AudioSourceComponent>().connect<&Scene::onAudioSourceDestroy>(this);
    }

    void Scene::setSkybox(const std::string& cubeMapId) {
        mSkyboxCubeMapId = cubeMapId;
    }

    bool Scene::hasSkybox() const {
        return !mSkyboxCubeMapId.empty();
    }

    std::shared_ptr<CubeMap> Scene::getSkybox() const {
        if (mSkyboxCubeMapId.empty()) {
            return nullptr;
        }
        return AssetManager::get().getAsset<CubeMap>(mSkyboxCubeMapId);
    }

    Entity Scene::getEntityFromHandle(entt::entity handle) {
        return Entity(handle, this);
    }

    Entity Scene::getEntity(std::string tag) {
        for (const auto& [entity, node] : mGraphNodes) {
            if (node->getTag() == tag) {
                return Entity(entity, this);
            }
        }
        return Entity();
    }

    std::shared_ptr<GraphNode> Scene::getGraphNode(entt::entity entity) {
        auto it = mGraphNodes.find(entity);
        if (it != mGraphNodes.end()) {
            return it->second;
        }
        return nullptr;
    }

    const std::shared_ptr<GraphNode> Scene::getGraphNode(entt::entity entity) const {
        auto it = mGraphNodes.find(entity);
        if (it != mGraphNodes.end()) {
            return it->second;
        }
        return nullptr;
    }

    Entity Scene::createEntity() {
        auto entity = mRegistry.create();
        auto graphNode = std::make_shared<GraphNode>(entity);
        mGraphNodes[entity] = graphNode;
        return Entity(entity, this);
    }

    Entity Scene::createEntity(const Entity& parent) {
        auto entity = createEntity();
        if (parent.isValid()) {
            setParent(entity, parent);
        }
        return entity;
    }

    void Scene::setParent(const Entity& child, const Entity& parent) {
        STRIKE_ASSERT(child.isValid(), "Scene::setParent called with invalid child entity");
        STRIKE_ASSERT(child.getHandle() != parent.getHandle(), "Entity cannot be parent of itself");
        STRIKE_ASSERT(!isAncestor(child, parent), "Scene::setParent would create circular hierarchy");

        auto childNode = getGraphNode(child.getHandle());
        auto parentNode = getGraphNode(parent.getHandle());

        childNode->setParent(parentNode);
    }

    void Scene::addChild(const Entity& parent, const Entity& child) {
        setParent(child, parent);
    }

    bool Scene::isAncestor(const Entity& ancestor, const Entity& descendant) const {
        STRIKE_ASSERT(ancestor.isValid(), "Scene::isAncestor called with invalid ancestor entity");
        STRIKE_ASSERT(descendant.isValid(), "Scene::isAncestor called with invalid descendant entity");

        auto ancestorNode = getGraphNode(ancestor.getHandle());
        auto descendantNode = getGraphNode(descendant.getHandle());
        STRIKE_ASSERT(ancestorNode, "Scene::isAncestor: ancestor graph node not found");
        STRIKE_ASSERT(descendantNode, "Scene::isAncestor: descendant graph node not found");

        auto currentNode = descendantNode->getParent();
        while (currentNode) {
            if (currentNode->getEntityId() == ancestor.getHandle()) {
                return true;
            }
            currentNode = currentNode->getParent();
        }
        return false;
    }

    bool Scene::isDescendant(const Entity& descendant, const Entity& ancestor) const {
        return isAncestor(ancestor, descendant);
    }

    void Scene::updateNodeTransforms(std::shared_ptr<GraphNode> node, bool parentDirty) {
        if (!node) return;
        if (!node->isActive()) return;

        entt::entity entity = node->getEntityId();
        if (!mRegistry.valid(entity)) {
            STRIKE_CORE_WARN("updateNodeTransforms: Invalid entity in graph node");
            return;
        }

        bool wasDirty = node->isDirty() || parentDirty;

        if (wasDirty) {
            glm::mat4 worldMatrix = node->getLocalMatrix();
            if (!node->isRoot()) {
                auto parent = node->getParent();
                if (parent) {
                    worldMatrix = parent->getWorldMatrix() * worldMatrix;
                }
            }
            node->mWorldMatrix = worldMatrix;
            node->mIsDirty = false;

            if (mRegistry.all_of<CameraComponent>(entity)) {
                auto& renderer = Renderer::get();
                auto& camera = mRegistry.get<CameraComponent>(entity);
                camera.update(worldMatrix, renderer.getWidth(), renderer.getHeight());
            }
        }

        for (auto& child : node->getChildren()) {
            updateNodeTransforms(child, wasDirty);
        }
    }

      void Scene::destroy(entt::entity entity) {
        if (!mRegistry.valid(entity)) {
            STRIKE_CORE_WARN("Attempted to destroy invalid entity");
            return;
        }

        auto node = getGraphNode(entity);
        if (node) {
           
            auto children = node->getChildren();
            for (auto& child : children) {
                destroy(child->getEntityId());
            }

            node->removeFromParent();
            mGraphNodes.erase(entity);
        }

        mRegistry.destroy(entity);
    }

    void Scene::shutdown() {
        mRegistry.on_destroy<PhysicsComponent>().disconnect<&Scene::onPhysicsComponentDestroy>(this);
        mRegistry.on_destroy<AudioSourceComponent>().disconnect<&Scene::onAudioSourceDestroy>(this);

        mGraphNodes.clear();
        mRegistry.clear();
    }

    void Scene::onPhysicsComponentDestroy(entt::registry& registry, entt::entity entity) {
        World::get().mPhysicsSystem->removePhysics(entity);
    }


    void Scene::onAudioSourceDestroy(entt::registry& registry, entt::entity entity) {
        World::get().mAudioSystem->stopEntity(entity);
    }

    void Scene::onUpdate(float dt) {
        for (auto& [entity, node] : mGraphNodes) {
            if (node->isRoot() && node->isActive()) {
                updateNodeTransforms(node);
            }
        }
    }

    void Scene::onRender() {
    }

}
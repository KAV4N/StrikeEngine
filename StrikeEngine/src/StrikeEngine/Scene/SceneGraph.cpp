#include "SceneGraph.h"
#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

namespace StrikeEngine {

    GraphNode::GraphNode(Entity entity, const std::string& id, const std::string& name, bool isRoot)
        : mEntity(entity), mId(id), mName(name), mPosition(0.0f),
        mRotation(1.0f, 0.0f, 0.0f, 0.0f), mScale(1.0f), mIsDirty(true),
        mIsRootNode(isRoot), mIsActive(true) {
    }

    SceneGraph::SceneGraph() = default;
    SceneGraph::~SceneGraph() = default;

    void SceneGraph::createRootNode(Entity entity, const std::string& id, const std::string& name) {
        if (!entity.isValid()) {
            std::cerr << "Error: Attempting to create root node with invalid entity" << std::endl;
            return;
        }
        if (nodes.find(entity) != nodes.end()) {
            std::cerr << "Error: Entity already exists in scene graph" << std::endl;
            return;
        }
        auto node = std::make_shared<GraphNode>(entity, id, name, true);
        try {
            nodes.emplace(entity, node);
            rootNode = node;
            std::cout << "Created root node for entity with ID: " << id << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in createRootNode: " << e.what() << std::endl;
        }
    }

    void SceneGraph::createNode(Entity entity, const std::string& id, const std::string& name) {
        if (!entity.isValid()) {
            std::cerr << "Error: Attempting to create node with invalid entity" << std::endl;
            return;
        }
        if (nodes.find(entity) != nodes.end()) {
            std::cerr << "Error: Entity already exists in scene graph" << std::endl;
            return;
        }
        auto node = std::make_shared<GraphNode>(entity, id, name, false);
        try {
            nodes.emplace(entity, node);
            std::cout << "Created node for entity with ID: " << id << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in createNode: " << e.what() << std::endl;
        }
    }

    void SceneGraph::removeNode(Entity entity) {
        auto node = getNode(entity);
        if (!node || node->isRoot()) return; // Cannot remove root node

        // Remove from parent's children
        if (auto parent = node->getParent()) {
            parent->removeChild(node);
        }

        // Re-parent children to this node's parent (or root if no parent)
        auto parent = node->getParent();
        if (!parent && rootNode) {
            parent = rootNode; // If no parent, use root
        }

        for (auto& child : node->getChildren()) {
            if (parent) {
                parent->addChild(child);
            }
        }

        // Remove from nodes map
        nodes.erase(entity);
    }

    bool SceneGraph::hasNode(Entity entity) const {
        return nodes.find(entity) != nodes.end();
    }

    bool SceneGraph::isRootNode(Entity entity) const {
        auto node = getNode(entity);
        return node && node->isRoot();
    }

    bool SceneGraph::isActive(Entity entity) const {
        auto node = getNode(entity);
        return node && node->isActive();
    }

    void SceneGraph::setParent(Entity child, Entity parent) {
        auto childNode = getNode(child);
        auto parentNode = getNode(parent);

        if (!childNode || !parentNode || child == parent) return;

        // Cannot reparent root node
        if (childNode->isRoot()) return;

        // Check for circular dependency
        if (isDescendant(parent, child)) return;

        // Remove from current parent
        if (auto currentParent = childNode->getParent()) {
            currentParent->removeChild(childNode);
        }

        // Set new parent
        parentNode->addChild(childNode);
    }

    void SceneGraph::removeParent(Entity child) {
        auto childNode = getNode(child);
        if (!childNode || childNode->isRoot()) return; // Cannot remove parent from root

        if (auto parent = childNode->getParent()) {
            parent->removeChild(childNode);
            childNode->setParent(nullptr);

            // Re-parent to root if available
            if (rootNode && childNode != rootNode) {
                rootNode->addChild(childNode);
            }
        }
    }

    void SceneGraph::addChild(Entity parent, Entity child) {
        setParent(child, parent);
    }

    void SceneGraph::removeChild(Entity parent, Entity child) {
        auto parentNode = getNode(parent);
        auto childNode = getNode(child);

        if (!parentNode || !childNode || childNode->isRoot()) return;

        parentNode->removeChild(childNode);
        childNode->setParent(nullptr);

        // Re-parent to root if available and not already root
        if (rootNode && childNode != rootNode) {
            rootNode->addChild(childNode);
        }
    }

    void SceneGraph::setPosition(Entity entity, const glm::vec3& position) {
        auto node = getNode(entity);
        if (node) {
            node->setPosition(position);
        }
    }

    void SceneGraph::setRotation(Entity entity, const glm::quat& rotation) {
        auto node = getNode(entity);
        if (node) {
            node->setRotation(rotation);
        }
    }

    void SceneGraph::setScale(Entity entity, const glm::vec3& scale) {
        auto node = getNode(entity);
        if (node) {
            node->setScale(scale);
        }
    }

    glm::vec3 SceneGraph::getPosition(Entity entity) const {
        auto node = getNode(entity);
        return node ? node->getPosition() : glm::vec3(0.0f);
    }

    glm::quat SceneGraph::getRotation(Entity entity) const {
        auto node = getNode(entity);
        return node ? node->getRotation() : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneGraph::getScale(Entity entity) const {
        auto node = getNode(entity);
        return node ? node->getScale() : glm::vec3(1.0f);
    }

    glm::vec3 SceneGraph::getWorldPosition(Entity entity) const {
        auto node = getNode(entity);
        if (!node) return glm::vec3(0.0f);

        glm::vec3 worldPos = node->getPosition();
        auto parent = node->getParent();

        while (parent) {
            worldPos = parent->getPosition() + parent->getRotation() * (parent->getScale() * worldPos);
            parent = parent->getParent();
        }

        return worldPos;
    }

    glm::quat SceneGraph::getWorldRotation(Entity entity) const {
        auto node = getNode(entity);
        if (!node) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        glm::quat worldRot = node->getRotation();
        auto parent = node->getParent();

        while (parent) {
            worldRot = parent->getRotation() * worldRot;
            parent = parent->getParent();
        }

        return worldRot;
    }

    glm::vec3 SceneGraph::getWorldScale(Entity entity) const {
        auto node = getNode(entity);
        if (!node) return glm::vec3(1.0f);

        glm::vec3 worldScale = node->getScale();
        auto parent = node->getParent();

        while (parent) {
            worldScale *= parent->getScale();
            parent = parent->getParent();
        }

        return worldScale;
    }

    glm::mat4 SceneGraph::getWorldMatrix(Entity entity) const {
        glm::vec3 worldPos = getWorldPosition(entity);
        glm::quat worldRot = getWorldRotation(entity);
        glm::vec3 worldScale = getWorldScale(entity);

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), worldPos);
        glm::mat4 rotation = glm::mat4_cast(worldRot);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), worldScale);

        return translation * rotation * scale;
    }

    Entity SceneGraph::getEntityByName(const std::string& name) const {
        for (const auto& [entity, node] : nodes) {
            if (node->getName() == name) {
                return entity;
            }
        }
        return Entity(); // Return invalid entity
    }

    Entity SceneGraph::getEntityById(const std::string& id) const {
        for (const auto& [entity, node] : nodes) {
            if (node->getId() == id) {
                return entity;
            }
        }
        return Entity(); // Return invalid entity
    }

    std::string SceneGraph::getEntityName(Entity entity) const {
        auto node = getNode(entity);
        return node ? node->getName() : "";
    }

    std::string SceneGraph::getEntityId(Entity entity) const {
        auto node = getNode(entity);
        return node ? node->getId() : "";
    }

    std::vector<Entity> SceneGraph::getChildren(Entity entity) const {
        auto node = getNode(entity);
        if (!node) return {};

        std::vector<Entity> children;
        for (const auto& child : node->getChildren()) {
            children.push_back(child->getEntity());
        }
        return children;
    }

    Entity SceneGraph::getParent(Entity entity) const {
        auto node = getNode(entity);
        if (!node) return Entity(); // Return invalid entity

        auto parent = node->getParent();
        return parent ? parent->getEntity() : Entity(); // Return invalid entity if no parent
    }

    bool SceneGraph::isAncestor(Entity ancestor, Entity descendant) const {
        return isDescendant(descendant, ancestor);
    }

    bool SceneGraph::isDescendant(Entity descendant, Entity ancestor) const {
        auto node = getNode(descendant);
        if (!node) return false;

        auto parent = node->getParent();
        while (parent) {
            if (parent->getEntity() == ancestor) {
                return true;
            }
            parent = parent->getParent();
        }
        return false;
    }

    std::vector<Entity> SceneGraph::getRootEntities() const {
        std::vector<Entity> roots;
        if (rootNode) {
            roots.push_back(rootNode->getEntity());
        }
        return roots;
    }

    Entity SceneGraph::getRootEntity() const {
        return rootNode ? rootNode->getEntity() : Entity();
    }

    void SceneGraph::markDirty(Entity entity) {
        auto node = getNode(entity);
        if (node) {
            node->markDirty();
        }
    }

    void SceneGraph::clearDirty(Entity entity) {
        auto node = getNode(entity);
        if (node) {
            node->clearDirty();
        }
    }

    bool SceneGraph::isDirty(Entity entity) const {
        auto node = getNode(entity);
        return node ? node->isDirty() : false;
    }

    void SceneGraph::updateTransforms() {
        for (auto& [entity, node] : nodes) {
            if (node->isDirty()) {
                node->clearDirty();
            }
        }
    }

    void SceneGraph::clear() {
        nodes.clear();
        rootNode.reset();
    }

    std::shared_ptr<GraphNode> SceneGraph::getNode(Entity entity) const {
        auto it = nodes.find(entity);
        return it != nodes.end() ? it->second : nullptr;
    }
}
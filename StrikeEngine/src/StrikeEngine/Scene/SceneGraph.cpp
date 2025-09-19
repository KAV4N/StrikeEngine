#include "SceneGraph.h"
#include "Entity.h"
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
#include <iostream>

namespace StrikeEngine {

    // GraphNode Implementation
    GraphNode::GraphNode(Entity entity, bool isRoot)
        : mEntity(entity), mIsRootNode(isRoot),
        mPosition(0.0f), mRotation(1.0f, 0.0f, 0.0f, 0.0f), mScale(1.0f),
        mLocalMatrix(1.0f), mWorldMatrix(1.0f) {
    }

    glm::vec3 GraphNode::getEulerRotation() {
        glm::vec3 eulerRad = glm::eulerAngles(mRotation);
        return normalizeEulerAngles(glm::degrees(eulerRad));
    }

    void GraphNode::setEulerRotation(const glm::vec3& eulerAngles) {
        glm::vec3 normalizedAngles = normalizeEulerAngles(eulerAngles);
        glm::vec3 eulerRad = glm::radians(normalizedAngles);
        mRotation = glm::normalize(glm::quat(eulerRad));
        mIsDirty = true;
    }

    void GraphNode::setRotationX(float angleDegrees) {
        glm::vec3 euler = getEulerRotation();
        euler.x = normalizeAngle(angleDegrees);
        setEulerRotation(euler);
    }

    void GraphNode::setRotationY(float angleDegrees) {
        glm::vec3 euler = getEulerRotation();
        euler.y = normalizeAngle(angleDegrees);
        setEulerRotation(euler);
    }

    void GraphNode::setRotationZ(float angleDegrees) {
        glm::vec3 euler = getEulerRotation();
        euler.z = normalizeAngle(angleDegrees);
        setEulerRotation(euler);
    }

    void GraphNode::rotateEuler(const glm::vec3& anglesDegrees) {
        glm::vec3 currentEuler = getEulerRotation();
        setEulerRotation(currentEuler + anglesDegrees);
    }

    void GraphNode::rotateX(float angleDegrees) {
        rotateEuler(glm::vec3(angleDegrees, 0.0f, 0.0f));
    }

    void GraphNode::rotateY(float angleDegrees) {
        rotateEuler(glm::vec3(0.0f, angleDegrees, 0.0f));
    }

    void GraphNode::rotateZ(float angleDegrees) {
        rotateEuler(glm::vec3(0.0f, 0.0f, angleDegrees));
    }

    void GraphNode::setParent(const std::shared_ptr<GraphNode>& newParent) {
        if (!mIsRootNode && newParent != shared_from_this()) {
            mParent = newParent;
            mIsDirty = true;
        }
    }

    void GraphNode::addChild(const std::shared_ptr<GraphNode>& child) {
        if (child && child != shared_from_this()) {
            auto it = std::find(mChildren.begin(), mChildren.end(), child);
            if (it == mChildren.end()) {
                mChildren.push_back(child);
                child->setParent(shared_from_this());
                child->mIsDirty = true;
            }
        }
    }

    void GraphNode::removeChild(const std::shared_ptr<GraphNode>& child) {
        auto it = std::remove(mChildren.begin(), mChildren.end(), child);
        if (it != mChildren.end()) {
            mChildren.erase(it, mChildren.end());
            if (child) {
                child->mParent.reset();
                child->mIsDirty = true;
            }
        }
    }

    void GraphNode::updateLocalMatrix() {
        if (mIsDirty) {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), mPosition);
            glm::mat4 rotation = glm::mat4_cast(mRotation);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), mScale);

            mLocalMatrix = translation * rotation * scale;
            mIsDirty = false;
        }
    }

    void GraphNode::updateWorldMatrix(const glm::mat4& parentWorldMatrix) {
        updateLocalMatrix();
        mWorldMatrix = parentWorldMatrix * mLocalMatrix;
    }

    float GraphNode::normalizeAngle(float angle) {
        angle = fmod(angle, 360.0f);
        return angle < 0.0f ? angle + 360.0f : angle;
    }

    glm::vec3 GraphNode::normalizeEulerAngles(const glm::vec3& angles) {
        return glm::vec3(
            normalizeAngle(angles.x),
            normalizeAngle(angles.y),
            normalizeAngle(angles.z)
        );
    }

    // SceneGraph Implementation
    SceneGraph::SceneGraph(Scene* scene, const std::string& rootId, const std::string& rootName) : mScene(scene) {
        createRootEntity(rootId, rootName);
    }

    void SceneGraph::createRootEntity(const std::string& rootId, const std::string& rootName) {
        Entity entity = createEntityInternal(rootId);
        entity.setName(rootName);
        auto rootNode = std::make_shared<GraphNode>(entity, true);
        mNodes[rootId] = rootNode;
        mRootNode = std::make_pair(rootId, rootNode);
    }

    Entity SceneGraph::createEntity(const std::string& id, const std::string& parentId) {
        if (id.empty()) {
            throw std::runtime_error("Entity ID cannot be empty");
        }

        if (entityExists(id)) {
            throw std::runtime_error("Entity with ID " + id + " already exists");
        }

        Entity entity = createEntityInternal(id);
        if (!entity.isValid()) {
            throw std::runtime_error("Failed to create entity with ID " + id);
        }

        auto node = std::make_shared<GraphNode>(entity, false);
        mNodes[id] = node;

        if (!parentId.empty() && entityExists(parentId)) {
            setParent(id, parentId);
        }
        else if (mRootNode.second) {
            setParent(id, mRootNode.first);
        }

        return entity;
    }

    Entity SceneGraph::getEntity(const std::string& id) {
        if (id.empty()) {
            return Entity();
        }

        auto it = mNodes.find(id);
        if (it != mNodes.end()) {
            return it->second->getEntity();
        }

        return Entity();
    }

    std::vector<Entity> SceneGraph::getEntitiesByName(const std::string& name) const {
        std::vector<Entity> result;

        for (const auto& pair : mNodes) {
            const auto& node = pair.second;
            if (node && node->getEntity().getName() == name) {
                result.push_back(node->getEntity());
            }
        }

        return result;
    }

    Entity SceneGraph::createEntityInternal(const std::string& id) {
        Entity entity;
        entity.mHandle = mRegistry.create();
        entity.mId = id;
        entity.mScene = mScene;
        entity.mSceneGraph = this;

        return entity;
    }

    bool SceneGraph::destroyEntity(const std::string& id) {
        if (!entityExists(id)) {
            return false;
        }

        if (isRoot(id)) {
            std::cerr << "Error: Cannot remove root node: " << id << std::endl;
            return false;
        }

        auto node = getNode(id);
        if (!node) {
            return false;
        }

        auto parent = node->getParent();
        if (parent) {
            parent->removeChild(node);
        }

        auto children = node->getChildren();
        for (auto& child : children) {
            if (child) {
                destroyEntity(child->getEntityId());
            }
        }

        if (node->getEntity().isValid()) {
            mRegistry.destroy(node->getEntity().mHandle);
        }

        mNodes.erase(id);

        return true;
    }

    Entity SceneGraph::getRootEntity() const {
        if (mRootNode.second) {
            return mRootNode.second->getEntity();
        }
        return Entity();
    }

    bool SceneGraph::isRoot(const std::string& id) const {
        return mRootNode.first == id;
    }

    bool SceneGraph::isActive(const std::string& id) const {
        auto node = getNode(id);
        return node ? node->isActive() : false;
    }

    bool SceneGraph::entityExists(const std::string& id) {
        return mNodes.find(id) != mNodes.end();
    }

    void SceneGraph::setParent(const std::string& childId, const std::string& parentId) {
        if (childId == parentId || childId.empty() || parentId.empty()) {
            return;
        }

        auto childNode = getNode(childId);
        auto parentNode = getNode(parentId);

        if (!childNode || !parentNode || childNode->isRoot()) {
            return;
        }

        if (isDescendant(parentId, childId)) {
            return;
        }

        auto currentParent = childNode->getParent();
        if (currentParent) {
            currentParent->removeChild(childNode);
        }

        parentNode->addChild(childNode);
    }

    void SceneGraph::addChild(const std::string& parentId, const std::string& childId) {
        setParent(childId, parentId);
    }

    void SceneGraph::removeChild(const std::string& parentId, const std::string& childId) {
        auto parentNode = getNode(parentId);
        auto childNode = getNode(childId);

        if (!parentNode || !childNode) {
            return;
        }

        parentNode->removeChild(childNode);

        if (mRootNode.second && !childNode->isRoot()) {
            mRootNode.second->addChild(childNode);
        }
    }

    std::vector<Entity> SceneGraph::getChildren(const std::string& id) const {
        std::vector<Entity> result;
        auto node = getNode(id);

        if (node) {
            const auto& children = node->getChildren();
            result.reserve(children.size());

            for (const auto& child : children) {
                if (child) {
                    result.push_back(child->getEntity());
                }
            }
        }

        return result;
    }

    Entity SceneGraph::getParent(const std::string& id) const {
        auto node = getNode(id);
        if (node) {
            auto parent = node->getParent();
            if (parent) {
                return parent->getEntity();
            }
        }
        return Entity();
    }

    bool SceneGraph::isAncestor(const std::string& ancestorId, const std::string& descendantId) const {
        if (ancestorId == descendantId) {
            return false;
        }

        auto descendantNode = getNode(descendantId);
        if (!descendantNode) {
            return false;
        }

        auto current = descendantNode->getParent();
        while (current) {
            if (current->getEntityId() == ancestorId) {
                return true;
            }
            current = current->getParent();
        }

        return false;
    }

    bool SceneGraph::isDescendant(const std::string& descendantId, const std::string& ancestorId) const {
        return isAncestor(ancestorId, descendantId);
    }

    void SceneGraph::updateTransforms() {
        if (mRootNode.second) {
            updateNodeTransforms(mRootNode.second);
        }
    }

    void SceneGraph::updateNodeTransforms(std::shared_ptr<GraphNode> node) {
        if (!node) {
            return;
        }

        auto parent = node->getParent();
        if (parent) {
            node->updateWorldMatrix(parent->getWorldMatrix());
        }
        else {
            node->updateWorldMatrix();
        }

        for (auto& child : node->getChildren()) {
            updateNodeTransforms(child);
        }
    }

    void SceneGraph::clear() {
        for (auto it = mNodes.begin(); it != mNodes.end();) {
            if (!isRoot(it->first)) {
                mRegistry.destroy(it->second->getEntity().mHandle);
                it = mNodes.erase(it);
            }
            else {
                ++it;
            }
        }

        if (mRootNode.second) {
            auto rootChildren = mRootNode.second->getChildren();
            rootChildren.clear();
        }
    }

    std::shared_ptr<GraphNode> SceneGraph::getNode(const std::string& id) const {
        auto it = mNodes.find(id);
        return it != mNodes.end() ? it->second : nullptr;
    }
}
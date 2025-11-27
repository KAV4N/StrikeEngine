#include "Scene.h"
#include "Entity.h"
#include "World.h"
#include "StrikeEngine/Graphics/Renderer.h"

#include "StrikeEngine/Scene/Components/LogicComponent.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"
#include "StrikeEngine/Scene/Systems/PhysicsSystem.h"

#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Asset/Types/Texture.h"

#include "Components/CameraComponent.h"

#include "StrikeEngine/Asset/AssetManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace StrikeEngine {

    void Sun::setColor(const glm::vec3& color) {
        mColor = glm::clamp(color, glm::vec3(0.0f), glm::vec3(255.0f));
    }

    const glm::vec3& Sun::getColor() const {
        return mColor;
    }

    void Sun::setIntensity(float intensity) {
        mIntensity = intensity;
    }

    float Sun::getIntensity() const {
        return mIntensity;
    }

    void Sun::setRotationEuler(const glm::vec3& eulerAngles) {
        mTransform.setRotationEuler(normalizeEulerAngles(eulerAngles));
    }

    void Sun::setRotation(const glm::quat& rotation) {
        mTransform.setRotation(rotation);
    }

    void Sun::rotateX(float angleDegrees) {
        mTransform.rotateX(angleDegrees);
    }

    void Sun::rotateY(float angleDegrees) {
        mTransform.rotateY(angleDegrees);
    }

    void Sun::rotateZ(float angleDegrees) {
        mTransform.rotateZ(angleDegrees);
    }

    void Sun::rotateQuaternion(float angleDegrees, glm::vec3 axis) {
        mTransform.rotateQuaternion(angleDegrees, axis);
    }

    glm::vec3 Sun::getRotationEuler() const {
        return mTransform.getRotationEuler();
    }

    glm::vec3 Sun::getDirection() const {
        return mTransform.getRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 Sun::normalizeEulerAngles(const glm::vec3& angles) const {
        auto normalizeAngle = [](float angle) {
            angle = fmod(angle + 180.0f, 360.0f);
            if (angle < 0.0f) angle += 360.0f;
            return angle - 180.0f;
        };
        
        return glm::vec3(
            normalizeAngle(angles.x),
            normalizeAngle(angles.y),
            normalizeAngle(angles.z)
        );
    }

    void Sun::setCastShadows(bool cast) {
        mCastShadows = cast;
    }

    bool Sun::getCastShadows() const {
        return mCastShadows;
    }

    void Sun::setShadowDistance(float distance) {
        mShadowDistance = distance;
    }

    float Sun::getShadowDistance() const {
        return mShadowDistance;
    }

    void Sun::setShadowArea(float area) {
        mShadowArea = area;
    }

    float Sun::getShadowArea() const {
        return mShadowArea;
    }

    glm::mat4 Sun::calculateLightSpaceMatrix(const glm::vec3& cameraPos) {
        updateFrustum(cameraPos);
        return mLightSpaceMatrix;
    }

    const Sun::Frustum& Sun::getFrustum() const {
        return mFrustum;
    }

    void Sun::updateFrustum(const glm::vec3& cameraPos) {
        glm::vec3 direction = getDirection();
        glm::vec3 lightPos = cameraPos - direction * (mShadowDistance * 0.5f);
        glm::vec3 targetPos = cameraPos + direction * (mShadowDistance * 0.5f);

        glm::mat4 lightProjection = glm::ortho(
            -mShadowArea, mShadowArea,
            -mShadowArea, mShadowArea,
            0.1f, mShadowDistance
        );

        glm::vec3 up = glm::abs(direction.y) > 0.99f ? 
            glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, targetPos, up);

        mLightSpaceMatrix = lightProjection * lightView;
        extractFrustumPlanes(mLightSpaceMatrix);
    }

    void Sun::extractFrustumPlanes(const glm::mat4& matrix) {
        mFrustum.planes[0] = glm::vec4(
            matrix[0][3] + matrix[0][0],
            matrix[1][3] + matrix[1][0],
            matrix[2][3] + matrix[2][0],
            matrix[3][3] + matrix[3][0]
        );
        mFrustum.planes[1] = glm::vec4(
            matrix[0][3] - matrix[0][0],
            matrix[1][3] - matrix[1][0],
            matrix[2][3] - matrix[2][0],
            matrix[3][3] - matrix[3][0]
        );
        mFrustum.planes[2] = glm::vec4(
            matrix[0][3] + matrix[0][1],
            matrix[1][3] + matrix[1][1],
            matrix[2][3] + matrix[2][1],
            matrix[3][3] + matrix[3][1]
        );
        mFrustum.planes[3] = glm::vec4(
            matrix[0][3] - matrix[0][1],
            matrix[1][3] - matrix[1][1],
            matrix[2][3] - matrix[2][1],
            matrix[3][3] - matrix[3][1]
        );
        mFrustum.planes[4] = glm::vec4(
            matrix[0][3] + matrix[0][2],
            matrix[1][3] + matrix[1][2],
            matrix[2][3] + matrix[2][2],
            matrix[3][3] + matrix[3][2]
        );
        mFrustum.planes[5] = glm::vec4(
            matrix[0][3] - matrix[0][2],
            matrix[1][3] - matrix[1][2],
            matrix[2][3] - matrix[2][2],
            matrix[3][3] - matrix[3][2]
        );

        for (auto& plane : mFrustum.planes) {
            float length = glm::length(glm::vec3(plane));
            if (length > 0.0f) plane /= length;
        }
    }

    Scene::Scene(const std::string& id, const std::filesystem::path& path)
        : mSun(std::make_shared<Sun>()),
        mId(id)
    {
    
        createRoot();
        setupComponentProtection();
    }

    Scene::~Scene() {
      
    }

    void Scene::setupComponentProtection() {
        mRegistry.on_destroy<HierarchyComponent>().connect<&Scene::onHierarchyDestroy>(this);
        mRegistry.on_destroy<TransformComponent>().connect<&Scene::onTransformDestroy>(this);
        mRegistry.on_destroy<TagComponent>().connect<&Scene::onTagDestroy>(this);
        mRegistry.on_construct<PhysicsComponent>().connect<&Scene::onPhysicsComponentCreate>(this);
        mRegistry.on_destroy<PhysicsComponent>().connect<&Scene::onPhysicsComponentDestroy>(this);
    }

    void Scene::setPhysicsSystem(PhysicsSystem* physicsSystem) {
        mPhysicsSystem = physicsSystem;
    }

    void Scene::setSkyboxCubeMap(const std::string& cubeMapId) {
        mSkyboxCubeMap = AssetManager::get().getCubeMap(cubeMapId);
    }

    std::shared_ptr<CubeMap> Scene::getSkyboxCubeMap() const {
        return mSkyboxCubeMap;
    }

    Entity Scene::getRoot() {
        return Entity(mRootEntity, this);
    }

    Entity Scene::getEntity(entt::entity handle) {
        return Entity(handle, this);
    }

    void Scene::createRoot() {
        mRootEntity = mRegistry.create();
        mRegistry.emplace<TransformComponent>(mRootEntity);
        auto& hierarchy = mRegistry.emplace<HierarchyComponent>(mRootEntity);
        mRegistry.emplace<TagComponent>(mRootEntity, "root");
        mRegistry.get<TransformComponent>(mRootEntity);
        
        hierarchy.mEntity = mRootEntity;
        hierarchy.mScene = this;
    }

    Entity Scene::createEntity() {
        auto entity = mRegistry.create();
        mRegistry.emplace<TransformComponent>(entity);
        auto& hierarchy = mRegistry.emplace<HierarchyComponent>(entity);
        mRegistry.emplace<TagComponent>(entity, "Entity");
        Entity entityWrapper(entity, this);

        hierarchy.mEntity = entityWrapper.getHandle();
        hierarchy.mScene = this;

        setParent(entityWrapper, getRoot());

        return entityWrapper;
    }

    Entity Scene::createEntity(const Entity& parent) {
        auto entity = createEntity();
        if (parent.isValid()) {
            setParent(entity, parent);
        }
        return entity;
    }

    void Scene::setParent(const Entity& child, const Entity& parent) {
        if (!child.isValid() || !parent.isValid()) {
            throw std::invalid_argument("Invalid entity");
        }
        
        entt::entity childHandle = child.getHandle();
        entt::entity parentHandle = parent.getHandle();
        
        if (childHandle == parentHandle) {
            throw std::invalid_argument("Entity cannot be parent of itself");
        }
        if (isAncestor(child, parent)) {
            throw std::invalid_argument("Circular hierarchy detected");
        }
        if (childHandle == mRootEntity) {
            throw std::invalid_argument("Scene Root entity can not be child");
        }

        auto& childHierarchy = mRegistry.get<HierarchyComponent>(childHandle);

        if (childHierarchy.getParentHandle()!=entt::null){
            entt::entity oldParentHandle = childHierarchy.getParentHandle();
            auto& oldParentChildren = mRegistry.get<HierarchyComponent>(oldParentHandle).mChildren;
            oldParentChildren.erase(std::remove(oldParentChildren.begin(), oldParentChildren.end(), childHandle), oldParentChildren.end());
        }
       
        
        childHierarchy.mParent = parentHandle;
        auto& parentHierarchy = mRegistry.get<HierarchyComponent>(parentHandle);
        parentHierarchy.mChildren.push_back(childHandle);

        parent.getTransform().markDirty();
    }

    void Scene::addChild(const Entity& parent, const Entity& child){
        setParent(child, parent);
    }

    bool Scene::isAncestor(const Entity& ancestor, const Entity& descendant) const {
       if (!ancestor.isValid() || !descendant.isValid()) return false;
        
        entt::entity ancestorHandle = ancestor.getHandle();
        entt::entity currentHandle = descendant.getHandle();
        
        while (currentHandle != entt::null) {
            const auto& hierarchy = mRegistry.get<HierarchyComponent>(currentHandle);
            if (hierarchy.isRoot()) break;
            currentHandle = hierarchy.getParentHandle();
            if (currentHandle == ancestorHandle) return true;
        }
        return false;

    }

    bool Scene::isDescendant(const Entity& descendant, const Entity& ancestor) const {
        return isAncestor(ancestor, descendant);
    }

    void Scene::updateTransforms() {
        updateNodeTransforms(mRootEntity);
    }

    void Scene::updateNodeTransforms(entt::entity entity, bool parentDirty) {
        if (!mRegistry.valid(entity) || !mRegistry.get<TransformComponent>(entity).isActive()) {
            return;
        }

        auto& transform = mRegistry.get<TransformComponent>(entity);
        auto& hierarchy = mRegistry.get<HierarchyComponent>(entity);
        bool wasDirty = transform.isDirty() || parentDirty;

        if (wasDirty) {
            glm::mat4 worldMatrix = transform.getLocalMatrix();
            if (!hierarchy.isRoot()) {
                entt::entity parentHandle = hierarchy.getParentHandle();
                const auto& parentTransform = mRegistry.get<TransformComponent>(parentHandle);
                worldMatrix = parentTransform.getWorldMatrix() * worldMatrix;
            }
            transform.setWorldMatrix(worldMatrix);

            // Update camera
            if (mRegistry.all_of<CameraComponent>(entity)) {
                auto& renderer = Renderer::get();
                auto& camera = mRegistry.get<CameraComponent>(entity);
                camera.update(worldMatrix, renderer.getWidth(), renderer.getHeight());
            }
        }

        const auto& childHandles = hierarchy.getChildrenHandles();
        for (entt::entity childHandle : childHandles) {
            updateNodeTransforms(childHandle, wasDirty);
        }
    }

    void Scene::destroy(entt::entity entity) {
        if (entity == mRootEntity) {
            return;
        }
        if (mRegistry.valid(entity) && mRegistry.all_of<HierarchyComponent>(entity)) {
            Entity entityWrapper(entity, this);

            auto& hierarchy = mRegistry.get<HierarchyComponent>(entity);
            auto childHandles = hierarchy.getChildrenHandles();
            for (entt::entity childHandle : childHandles) {
                destroy(childHandle);
            }
        }

        mRegistry.destroy(entity);
    }

    void Scene::shutdown() {
        mRegistry.on_destroy<HierarchyComponent>().disconnect<&Scene::onHierarchyDestroy>(this);
        mRegistry.on_destroy<TransformComponent>().disconnect<&Scene::onTransformDestroy>(this);
        mRegistry.on_destroy<TagComponent>().disconnect<&Scene::onTagDestroy>(this);
        mRegistry.on_destroy<PhysicsComponent>().disconnect<&Scene::onPhysicsComponentDestroy>(this);
        mRegistry.clear();
    }

    void Scene::onHierarchyDestroy(entt::registry& registry, entt::entity entity) {
        if (!mRegistry.valid(entity)) return;

        if (!mRegistry.all_of<HierarchyComponent>(entity)) {
            auto& hierarchy = mRegistry.emplace<HierarchyComponent>(entity);
            Entity entityWrapper(entity, this);

            hierarchy.mEntity = entityWrapper.getHandle();
            hierarchy.mScene = this;

            setParent(entityWrapper, getRoot());
        }
    }

    void Scene::onTransformDestroy(entt::registry& registry, entt::entity entity) {
        if (!mRegistry.valid(entity)) return;

        if (!mRegistry.all_of<TransformComponent>(entity)) {
            mRegistry.emplace<TransformComponent>(entity);
        }
    }

    void Scene::onTagDestroy(entt::registry& registry, entt::entity entity) {
        if (!mRegistry.valid(entity)) return;

        if (!mRegistry.all_of<TagComponent>(entity)) {
            mRegistry.emplace<TagComponent>(entity, "Entity");
        }
    }

    void Scene::onPhysicsComponentDestroy(entt::registry& registry, entt::entity entity) {
        // Clean up physics resources when PhysicsComponent is removed
        if (mPhysicsSystem) {
            mPhysicsSystem->removePhysics(entity);
        }
    }

    
    void Scene::onPhysicsComponentCreate(entt::registry& registry, entt::entity entity) {
        /*
        if (mPhysicsSystem) {
            mPhysicsSystem->createRigidBody(entity);
        }
            */
    }

    void Scene::onUpdate(float dt) {
        updateTransforms();
    }

    void Scene::onRender() {
    }

} // namespace StrikeEngine
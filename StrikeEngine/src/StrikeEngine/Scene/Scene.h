#pragma once

#include "StrikeEngine/Events/Event.h"

#include "Components/TransformComponent.h"
#include "Components/HierarchyComponent.h"
#include "Components/TagComponent.h"

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

namespace StrikeEngine {

    class Asset;
    class CubeMap;
    class Entity;
    class PhysicsSystem;
    
    class Sun {
    public:
        struct Frustum {
            std::array<glm::vec4, 6> planes;
        };

        Sun() = default;
        ~Sun() = default;

        void setColor(const glm::vec3& color);
        const glm::vec3& getColor() const;

        void setIntensity(float intensity);
        float getIntensity() const;

        void setRotationEuler(const glm::vec3& eulerAngles);
        void setRotation(const glm::quat& rotation);

        void rotateQuaternion(float angleDegrees, glm::vec3 axis);
        void rotateX(float angleDegrees);
        void rotateY(float angleDegrees);
        void rotateZ(float angleDegrees);

        glm::vec3 getRotationEuler() const;
        glm::vec3 getDirection() const;

        void setCastShadows(bool cast);
        bool getCastShadows() const;

        void setShadowDistance(float distance);
        float getShadowDistance() const;

        void setShadowArea(float area);
        float getShadowArea() const;

        glm::mat4 calculateLightSpaceMatrix(const glm::vec3& cameraPos);
        const Frustum& getFrustum() const;

        void updateFrustum(const glm::vec3& cameraPos);

    private:
        void extractFrustumPlanes(const glm::mat4& matrix);
        glm::vec3 normalizeEulerAngles(const glm::vec3& angles) const;

    private:
        glm::vec3 mColor = glm::vec3(1.0f);
        float mIntensity = 1.0f;

        TransformComponent mTransform;

        bool mCastShadows = false;
        float mShadowDistance = 100.0f;
        float mShadowArea = 50.0f;

        glm::mat4 mLightSpaceMatrix{1.0f};
        Frustum mFrustum;
    };

    class Scene final {
    public:
        Scene(const std::string& id, const std::filesystem::path& path);
        ~Scene();

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        // Entity creation and retrieval
        Entity createEntity();
        Entity createEntity(const Entity& parent);
        Entity getRoot();
        Entity getEntity(entt::entity handle);


        // Hierarchy operations
        void setParent(const Entity& child, const Entity& parent);
        void addChild(const Entity& parent, const Entity& child);

        
        void setSkyboxCubeMap(const std::string& cubeMapId);
        std::shared_ptr<CubeMap> getSkyboxCubeMap() const;

        // Update operations
        void updateTransforms();

        // Management
        void shutdown();
        void destroy(entt::entity entity);

        bool isAncestor(const Entity& ancestor, const Entity& descendant) const;
        bool isDescendant(const Entity& descendant, const Entity& ancestor) const;
        
        entt::registry& getRegistry() { return mRegistry; }
        const entt::registry& getRegistry() const { return mRegistry; }

        template<typename Component, typename... Args>
        decltype(auto) addComponent(entt::entity entity, Args&&... args) {
            return mRegistry.emplace<Component>(entity, std::forward<Args>(args)...);
        }

        template<typename Component>
        void removeComponent(entt::entity entity) {
            static_assert(!std::is_same_v<Component, HierarchyComponent> && 
                          !std::is_same_v<Component, TransformComponent> &&
                          !std::is_same_v<Component, TagComponent>,
                          "Cannot remove core components (HierarchyComponent, TransformComponent, TagComponent)");
            mRegistry.remove<Component>(entity);
        }

        template<typename Component>
        Component& getComponent(entt::entity entity) {
            return mRegistry.get<Component>(entity);
        }

        template<typename Component>
        const Component& getComponent(entt::entity entity) const {
            return mRegistry.get<Component>(entity);
        }

        template<typename Component>
        bool hasComponent(entt::entity entity) const {
            return mRegistry.all_of<Component>(entity);
        }

        template<typename... Components>
        auto view() {
            return mRegistry.view<Components...>();
        }

        template<typename... Components>
        auto view() const {
            return mRegistry.view<Components...>();
        }

        void onUpdate(float dt);
        void onRender();

                
       
        entt::entity getRootHandle() const { return mRootEntity; }
        std::shared_ptr<Sun> getSun() const { return mSun; }
        const std::string& getId() {return mId;} 

    private:
        friend class Entity;
        friend class HierarchyComponent;  
        friend class World;  

        void createRoot();
        void updateNodeTransforms(entt::entity entity, bool parentDirty = false);
        
        void onHierarchyDestroy(entt::registry& registry, entt::entity entity);
        void onTransformDestroy(entt::registry& registry, entt::entity entity);
        void onTagDestroy(entt::registry& registry, entt::entity entity);
        void onPhysicsComponentDestroy(entt::registry& registry, entt::entity entity);
        void onPhysicsComponentCreate(entt::registry& registry, entt::entity entity);
        void setupComponentProtection();

        void setPhysicsSystem(PhysicsSystem* physicsSystem);

    private:
        std::string mId;
        entt::registry mRegistry;

        std::shared_ptr<CubeMap> mSkyboxCubeMap;
        std::shared_ptr<Sun> mSun;

        entt::entity mRootEntity = entt::null;
        PhysicsSystem* mPhysicsSystem = nullptr;
    };

}
#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h" 

#include "Components/ModelComponent.h"
#include "Components/ShadowCasterComponent.h"
#include "Components/LightComponents.h"
#include "StrikeEngine/Scene/RenderCommand.h"

#include "StrikeEngine/Scene/Systems/TransformSystem.h"

namespace StrikeEngine {


    Scene::Scene() {
        m_ActiveView = new ScreenPanel();
        m_ViewList.push_back(m_ActiveView);
        m_Skybox = std::make_unique<Skybox>();
    }


    Scene::~Scene() {
        m_ActiveView = nullptr;
        for (ScreenPanel* view : m_ViewList)
            delete view; 
    }



    std::vector<Entity>  Scene::GetDirectionalLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<DirectionalLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
            
        }
        return lights;
    }

    std::vector<Entity>  Scene::GetPointLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<PointLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
        }
        return lights;
    }

    std::vector<Entity>  Scene::GetSpotLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<SpotLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));    
        }
        return lights;
    }

    std::vector<Entity> Scene::GetShadowCastingLights() const {
        std::vector<Entity> shadowCasters;
        auto view = m_Registry.view<ShadowCasterComponent>();
        for (auto entity : view) {
            shadowCasters.emplace_back(entity, const_cast<Scene*>(this));
        }
        return shadowCasters;
    }
    Entity Scene::CreateEntity(Model* model, const std::string& name) {
        // Create the parent entity

        Entity parentEntity(m_Registry.create(), this);

        // Add TransformComponent and ParentComponent to parent entity
        parentEntity.AddComponent<TransformComponent>();
        parentEntity.AddComponent<VisibleTag>();

        parentEntity.AddComponent<ParentComponent>(Entity(entt::null, this));
        parentEntity.AddComponent<ChildrenComponent>();

        Shader* defaultShader = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader());
        // Add children entities based on the model
        for (Mesh* mesh : model->GetMeshes()) {
            Entity childEntity(m_Registry.create(), this);

            childEntity.AddComponent<TransformComponent>();
            childEntity.AddComponent<MeshComponent>(mesh, defaultShader);
            childEntity.AddComponent<VisibleTag>();

            // Add ParentComponent to child entity
            childEntity.AddComponent<ParentComponent>(parentEntity);
            childEntity.AddComponent<ChildrenComponent>();

            // Add child to parent's ChildrenComponent list
            auto& children = parentEntity.GetComponent<ChildrenComponent>().Children;
            children.push_back(childEntity);
        }

        return parentEntity;
    }


    std::vector<Entity> Scene::GetAllEntitiesWithModelComponent() const {
        /*
        std::vector<Entity> entities;
        auto view = m_Registry.view<ModelComponent>();
        for (auto entity : view) {
            entities.emplace_back(entity, const_cast<Scene*>(this));
        }
        return entities;
        */
        std::vector<Entity> test;
        return test;
    }



    void Scene::RemoveEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

    Entity Scene::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<DirectionalLightComponent>(direction, color, intensity);
        return entity;
    }

    Entity Scene::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<PointLightComponent>(position, color, intensity, radius);
        return entity;
    }

    Entity Scene::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<SpotLightComponent>(position, direction, cutoff, color, intensity);
        return entity;
    }

    Entity Scene::CreateShadowCaster(Entity lightEntity)
    {
        if (!lightEntity.HasComponent<DirectionalLightComponent>() &&
            !lightEntity.HasComponent<PointLightComponent>() &&
            !lightEntity.HasComponent<SpotLightComponent>()) {
            throw std::runtime_error("Entity must have a light component to create a ShadowCasterComponent.");
        }

        lightEntity.AddComponent<ShadowCasterComponent>();
        return lightEntity;
    }

    

    void Scene::SubmitMesh() {
        auto viewMesh = m_Registry.view<MeshComponent, TransformComponent, ParentComponent, VisibleTag>();

        for (auto meshEntity : viewMesh) {
            auto& meshComp = viewMesh.get<MeshComponent>(meshEntity);

            Mesh* mesh = meshComp.MeshObj;
            Shader* shader = meshComp.ShaderObj;

            Entity entity(meshEntity, static_cast<Scene*>(this));

            glm::mat4 transMesh = TransformSystem::CalculateTransformMatrix(entity);
            glm::mat4 transWorld = TransformSystem::CalculateTransformMatrix(viewMesh.get<ParentComponent>(meshEntity).Parent);

            glm::mat4 transMeshWorld = transWorld * transMesh;
            Renderer::SubmitMesh(shader, {mesh, transMeshWorld });
        }
       
    }

    void Scene::SubmitSkybox() {
        Renderer::SubmitSkybox(m_Skybox.get());
    }


    void Scene::OnUpdate(float deltaTime) {
        
        SubmitSkybox();
        SubmitMesh();

    }


    void Scene::OnEvent(Event& event) {
        m_ActiveView->OnEvent(event);

    }

    void Scene::RenderScene(FrameBuffer* frameBuffer) {
        m_ActiveView->OnRender();

        frameBuffer->Bind();
        m_ActiveView->CompositeView();
        frameBuffer->Unbind();

    }

}

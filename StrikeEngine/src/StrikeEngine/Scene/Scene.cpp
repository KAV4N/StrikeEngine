#include "strikepch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Scene.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h" 

#include "Components/ModelComponent.h"
#include "Components/ShadowCasterComponent.h"
#include "Components/LightComponents.h"
#include "StrikeEngine/Scene/RenderCommand.h"

#include "StrikeEngine/Scene/Systems/TransformSystem.h"
#include <glm/gtx/matrix_decompose.hpp>

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
        Entity parentEntity(m_Registry.create(), this);

        parentEntity.AddComponent<TransformComponent>();
        parentEntity.AddComponent<VisibleComponent>();
        parentEntity.AddComponent<RootModelComponent>();

        Shader* defaultShader = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader());

        for (Mesh* mesh : model->GetMeshes()) {
            Entity childEntity(m_Registry.create(), this);

            childEntity.AddComponent<TransformComponent>();
            childEntity.AddComponent<MeshComponent>(mesh, defaultShader);
            childEntity.AddComponent<VisibleComponent>();
            childEntity.AddComponent<ParentComponent>(parentEntity);

            auto& children = parentEntity.GetComponent<RootModelComponent>().Children;
            children.push_back(childEntity);
        }

        return parentEntity;
    }

    void Scene::RemoveEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

    Entity Scene::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<DirectionalLightComponent>(color, intensity);
        entity.AddComponent<VisibleComponent>();
        entity.AddComponent<ParentComponent>();
        entity.AddComponent<TransformComponent>(glm::vec3(0.f), direction);
        return entity;
    }

    Entity Scene::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<PointLightComponent>(color, intensity, radius);
        entity.AddComponent<VisibleComponent>();
        entity.AddComponent<ParentComponent>();
        entity.AddComponent<TransformComponent>(position);
        return entity;
    }

    Entity Scene::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<SpotLightComponent>(color, intensity, cutoff);
        entity.AddComponent<VisibleComponent>();
        entity.AddComponent<ParentComponent>();
        entity.AddComponent<TransformComponent>(position, direction);
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

    void Scene::SubmitModelMesh(const std::vector<Entity>& modelChildren, const glm::mat4& rootTransform) {
        for (const Entity& entity : modelChildren) {
            const auto& entityTransformComp = entity.GetComponent<TransformComponent>();
            glm::mat4 entityTransformMatrix = TransformSystem::CalculateTransformMatrix(entityTransformComp);
            glm::mat4 worldTransform = rootTransform * entityTransformMatrix;

            if (entity.HasComponent<ModelComponent>()) {
                SubmitModelMesh(entity.GetComponent<ModelComponent>().Children, worldTransform);
            }
            
            if (entity.HasComponent<MeshComponent>()) {
                const auto& meshComp = entity.GetComponent<MeshComponent>();
                Mesh* mesh = meshComp.MeshObj;
                Shader* shader = meshComp.ShaderObj;

                Renderer::SubmitMesh(shader, { mesh, worldTransform });
            }

            if (entity.HasComponent<SpotLightComponent>()) {
                
                auto& light = entity.GetComponent<SpotLightComponent>();
                auto& lightTransf = entity.GetComponent<TransformComponent>(); 

                glm::mat4 lightModelTransform = TransformSystem::CalculateTransformMatrix(lightTransf);
                glm::mat4 test = rootTransform * lightModelTransform;

                lightTransf.Position = glm::vec3(rootTransform[3]) * lightTransf.Position;


                //ightTransf.Rotation = glm::eulerAngles(glm::quat_cast(rotationMatrix * glm::mat3(TransformSystem::CalculateRotationMatrix(lightTransf))));

                //StrikeEngine::LightManager::SetSpotLightsDirty();

            }
        }
    }


    void Scene::SubmitMesh() {
        auto rootView = m_Registry.view<TransformComponent, RootModelComponent, VisibleComponent>();
        for (auto entity : rootView) {
            const auto& rootTransform = rootView.get<TransformComponent>(entity);
            const auto& rootModelComp = rootView.get<RootModelComponent>(entity);

            SubmitModelMesh(rootModelComp.Children,
                TransformSystem::CalculateTransformMatrix(rootTransform));
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
        Renderer::Get()->Resize(m_ActiveView->GetFrameBuffer()->GetWidth(), m_ActiveView->GetFrameBuffer()->GetHeight());
        m_ActiveView->OnRender();
        
        Renderer::Resize(frameBuffer->GetWidth(), frameBuffer->GetHeight());
        frameBuffer->Bind();
        m_ActiveView->CompositeView();
        frameBuffer->Unbind();

    }

}

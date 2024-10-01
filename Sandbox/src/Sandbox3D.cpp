#include "strikepch.h"
#include "Sandbox3D.h"
#include <chrono>

Sandbox3D::Sandbox3D()
	: m_World(nullptr), m_LastFrameTime(0.0f)
{
    // Load models
    StrikeEngine::Model* model = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj");
  
    StrikeEngine::Model* model2 = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/penguin/PenguinBaseMesh.obj");

    StrikeEngine::Model* model3 = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/box/circle.obj");

    m_TestEntity1 = StrikeEngine::World::Get()->GetActiveScene()->CreateEntity(model);
    m_TestEntity2 = StrikeEngine::World::Get()->GetActiveScene()->CreateEntity(model2);
    StrikeEngine::Entity entity3 = StrikeEngine::World::Get()->GetActiveScene()->CreateEntity(model3);
    
    auto& transf1 = m_TestEntity1.GetComponent<StrikeEngine::TransformComponent>();
    auto& transf2 = m_TestEntity2.GetComponent<StrikeEngine::TransformComponent>();
    auto& transf3 = entity3.GetComponent<StrikeEngine::TransformComponent>();
    
    transf1.Rotation = glm::vec3(270.f, 0.f, 0.f);
    transf1.Position = glm::vec3(1.f, 0.f, 0.f);

    transf2.Position = glm::vec3(5.f, 0.f, 0.f);

    transf3.Scale = glm::vec3(10.f);

    /*

    StrikeEngine::Entity light = StrikeEngine::LightManager::Get()->CreateSpotLight(
        glm::vec3(2.0f, 2.0f, 1.0f),
        glm::vec3(-0.5f, 0.2f, -0.5f),
        cos(glm::radians(60.0f)),
        glm::vec3(1.0f, 1.0f, 1.0f),
        2.0f);
        */
    StrikeEngine::Entity light1 = StrikeEngine::LightManager::Get()->CreateSpotLight(
        glm::vec3(0.0f, 2.0f, 1.0f),
        glm::vec3(0.0f, 0.2f, -1.f),
        cos(glm::radians(60.f)),
        glm::vec3(1.0f, 0.0f, 0.0f),
        1000.0f);
    
    StrikeEngine::Entity light2 = StrikeEngine::LightManager::Get()->CreateDirectionalLight(
        glm::vec3(0.f, 4.f, 6.f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        2.f);

    m_TestEntity1.GetComponent<StrikeEngine::RootModelComponent>().Children.push_back(light1);

}

void Sandbox3D::OnAttach()
{
	m_World = StrikeEngine::World::Get();
    StrikeEngine::ScreenPanel* activeView = m_World->GetActiveScene()->GetActiveView();
    StrikeEngine::ScreenPanel* pipView = activeView->AddScreen(glm::vec2(0.25f),glm::vec2(0.25f));
    StrikeEngine::ScreenPanel* pipView2 = activeView->AddScreen(glm::vec2(0.25f,-0.25f), glm::vec2(0.25f));

    StrikeEngine::Camera* activeCamera = activeView->GetCamera();
    StrikeEngine::Camera* pipCamera = pipView->GetCamera();

    pipCamera->SetPosition(glm::vec3(2.f,1.f,1.f));
    activeCamera->SetPosition(glm::vec3(0.0f, 2.f, 6.f));
    activeCamera->SetRotation(glm::vec3(0.f, -60.f,0.f));

    auto& transfLight = m_TestEntity1.GetComponent<StrikeEngine::RootModelComponent>().Children[m_TestEntity1.GetComponent<StrikeEngine::RootModelComponent>().Children.size() - 1].GetComponent<StrikeEngine::TransformComponent>();
    transfLight.Rotation = glm::vec3(0.f, 1.f, 0.f);
}

void Sandbox3D::OnDetach()
{
	m_World = nullptr;
}
  
void Sandbox3D::OnUpdate(float deltaTime)
{
    
    auto& transfLight = m_TestEntity1.GetComponent<StrikeEngine::RootModelComponent>().Children[m_TestEntity1.GetComponent<StrikeEngine::RootModelComponent>().Children.size() - 1].GetComponent<StrikeEngine::TransformComponent>();
 
    transfLight.Rotation += glm::vec3(0.f, 0.f, 1.f);
    StrikeEngine::LightManager::SetSpotLightsDirty();
    
    auto& transf1 = m_TestEntity1.GetComponent<StrikeEngine::RootModelComponent>().Children[0].GetComponent<StrikeEngine::TransformComponent>();
    transf1.Position = glm::vec3(20.f, 0.f, 0.f);
    transf1.Scale = glm::vec3(2.f,1.f,1.f);


    auto& transf3 = m_TestEntity1.GetComponent<StrikeEngine::TransformComponent>();
    transf3.Rotation += glm::vec3(1.f, 0.f, 0.f);
//    transf1.Scale = glm::vec3(2.f, 1.f, 1.f);


    auto& transf2 = m_TestEntity2.GetComponent<StrikeEngine::TransformComponent>();
    transf2.Rotation += glm::vec3(1.f, 0.f, 1.f);
    transf2.Scale = glm::vec3(2.f);

}

void Sandbox3D::OnImGuiRender()
{

}

void Sandbox3D::OnEvent(StrikeEngine::Event& event)
{

}

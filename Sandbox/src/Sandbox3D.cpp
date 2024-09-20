#include "strikepch.h"
#include "Sandbox3D.h"
#include <chrono>

Sandbox3D::Sandbox3D()
	: m_World(nullptr), m_LastFrameTime(0.0f)
{
    // Load models
    StrikeEngine::Model* model = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj");
    //model->SetShader(ShaderManager::Get()->GetShader("ShinyShader"));

    StrikeEngine::Model* model2 = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/penguin/PenguinBaseMesh.obj");
    //model2->SetShader(ShaderManager::Get()->GetShader("ComicShader"));

    StrikeEngine::Model* model3 = StrikeEngine::ModelManager::Get()->LoadModel("assets/objects/box/circle.obj");
    //model3->SetShader(ShaderManager::Get()->GetShader("ShinyShader"));

    StrikeEngine::Entity entity = StrikeEngine::World::Get()->GetActiveScene()->CreateEntity(model);
    StrikeEngine::Entity entity2 = StrikeEngine::World::Get()->GetActiveScene()->CreateEntity(model2);
    StrikeEngine::Entity entity3 = StrikeEngine::World::Get()->GetActiveScene()->CreateEntity(model3);

    StrikeEngine::Entity light = StrikeEngine::LightManager::Get()->CreateSpotLight(
        glm::vec3(2.0f, 2.0f, 1.0f),
        glm::vec3(-0.5f, 0.2f, -0.5f),
        cos(glm::radians(60.0f)),
        glm::vec3(1.0f, 1.0f, 1.0f),
        2.0f);
    StrikeEngine::LightManager::Get()->CreateShadowCaster(light);

    StrikeEngine::Entity light1 = StrikeEngine::LightManager::Get()->CreateSpotLight(
        glm::vec3(0.0f, 2.0f, 1.0f),
        glm::vec3(0.0f, 0.2f, -1.f),
        cos(glm::radians(60.f)),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    StrikeEngine::LightManager::Get()->CreateShadowCaster(light1);

    StrikeEngine::Entity light2 = StrikeEngine::LightManager::Get()->CreateDirectionalLight(
        glm::vec3(0.f, -4.f, -6.f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        0.8f);
    StrikeEngine::LightManager::Get()->CreateShadowCaster(light2);
}

void Sandbox3D::OnAttach()
{
	m_World = StrikeEngine::World::Get();
}

void Sandbox3D::OnDetach()
{
	m_World = nullptr;
}
  
void Sandbox3D::OnUpdate(float deltaTime)
{

}

void Sandbox3D::OnImGuiRender()
{
}

void Sandbox3D::OnEvent(StrikeEngine::Event& event)
{

}

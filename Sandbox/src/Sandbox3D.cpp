#include "strikepch.h"
#include "Sandbox3D.h"
#include <chrono>

Sandbox3D::Sandbox3D()
	: m_World(nullptr), m_LastFrameTime(0.0f)
{

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
	StrikeEngine::Entity sceneCamera = m_World->GetActiveScene()->GetCameraEntity();
	StrikeEngine::CameraSystem::OnUpdate(sceneCamera, deltaTime);
	m_World->OnUpdate(deltaTime);
}

void Sandbox3D::OnImGuiRender()
{
}

void Sandbox3D::OnEvent(StrikeEngine::Event& event)
{
	StrikeEngine::Entity sceneCamera = m_World->GetActiveScene()->GetCameraEntity();

	StrikeEngine::CameraSystem::OnEvent(sceneCamera, event);
}

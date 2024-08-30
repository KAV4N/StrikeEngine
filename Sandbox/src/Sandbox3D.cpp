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
  
void Sandbox3D::OnUpdate()
{
	StrikeEngine::Entity sceneCamera = m_World->GetActiveScene()->GetCameraEntity();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime.time_since_epoch()).count();


	float deltaTime = time - m_LastFrameTime;

	m_LastFrameTime = time;

	//StrikeEngine::CameraSystem::OnUpdate(sceneCamera);
	StrikeEngine::CameraSystem::OnUpdate(sceneCamera, deltaTime);
	m_World->Update();
}

void Sandbox3D::OnImGuiRender()
{
}

void Sandbox3D::OnEvent(StrikeEngine::Event& event)
{
	StrikeEngine::Entity sceneCamera = m_World->GetActiveScene()->GetCameraEntity();

	StrikeEngine::CameraSystem::OnEvent(sceneCamera, event);
}

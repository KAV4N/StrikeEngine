#include "EditorLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace StrikeEngine {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"),
		m_ViewportPanel(new ScreenPanel()),
		m_CameraController(m_ViewportPanel->GetCamera())
	{
	}

	EditorLayer::~EditorLayer() {
		delete m_ViewportPanel;
	}

	void EditorLayer::OnAttach()
	{
		
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		m_CameraController.OnUpdate(deltaTime);
	}

	void EditorLayer::OnRender() {
		m_ViewportPanel->OnRender();
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Rendered Texture");
		ImGui::Text("This is the rendered texture:");
		ImGui::Image((void*)(intptr_t)m_ViewportPanel->GetFrameBuffer()->GetColorAttachment(), ImVec2(1280, 720));
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}

}
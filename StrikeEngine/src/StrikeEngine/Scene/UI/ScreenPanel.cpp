#include "strikepch.h"
#include "ScreenPanel.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"

namespace StrikeEngine {
	ScreenPanel::ScreenPanel(glm::vec2 position, glm::vec2 size)
		: m_Camera(new Camera(16.f / 9.f, 70.f, 0.1f, 1000.f)),
		m_FrameBuffer(new FrameBuffer(1280, 720)),
		m_Size(size),
		m_Position(position)
	{
	}
	ScreenPanel::~ScreenPanel()
	{
		delete m_FrameBuffer;
		delete m_Camera;
		auto& uiLayers = m_WidgetStack.GetAllLayers();
		auto& screenLayers = m_ScreenViewStack.GetAllLayers();
		for (Layer* ui : uiLayers)
			delete ui;

		for (Layer* screen : screenLayers)
			delete screen;
	}

	void ScreenPanel::OnEvent(Event& e)
	{
	}

	ScreenPanel* ScreenPanel::AddScreen(glm::vec2 position, glm::vec2 size) {
		ScreenPanel* screenPanel = new ScreenPanel(position, size);
		m_ScreenViewStack.PushLayer(screenPanel);
		return screenPanel;
	}

	void ScreenPanel::OnUpdate(float deltaTime)
	{

	}



	void ScreenPanel::OnRender()
	{
		m_FrameBuffer->Bind();
		Renderer::Get()->Present(m_Camera);
		for (Layer* widget : m_WidgetStack)
			widget->OnRender();
		m_FrameBuffer->Unbind();

		for (ScreenPanel* screen : m_ScreenViewStack)
			screen->OnRender();
	}

	void ScreenPanel::CompositeView() {
		Renderer::Get()->DrawTexturedQuad(m_Position, m_Size, m_FrameBuffer->GetColorAttachment());

		for (ScreenPanel* screen : m_ScreenViewStack)
			screen->CompositeView();
	}
}
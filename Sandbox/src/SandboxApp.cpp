#include "strikepch.h"
#include <StrikeEngine.h>

#include "imgui.h"

class TestLayer : public StrikeEngine::Layer
{
public:
	TestLayer()
		: Layer("Test")
	{

	}
	void OnUpdate() override
	{
		if (StrikeEngine::Input::IsKeyPressed(STRIKE_KEY_TAB))
			STRIKE_TRACE("Tab key is pressed (poll)!");
	}
	
	
	void OnImGuiRender() override
	{
		ImGui::Begin("Test2");
		ImGui::Text("aasdasda");
		ImGui::End();
	}
	
	
	void OnEvent(StrikeEngine::Event& event) override
	{
		if (event.GetEventType() == StrikeEngine::EventType::KeyPressed)
		{
			StrikeEngine::KeyPressedEvent& e = (StrikeEngine::KeyPressedEvent&)event;
			
			if (e.GetKeyCode() == STRIKE_KEY_TAB)
				STRIKE_TRACE("Tab key is pressed (event)!");
			STRIKE_TRACE("{0}", (char)e.GetKeyCode());
			
		}
	}
};

class Sandbox : public StrikeEngine::Application 
{

public:
	Sandbox() 
	{
		PushLayer(new TestLayer());
		//PushOverlay(new StrikeEngine::ImGuiLayer());
	}
	~Sandbox() 
	{

	}
};

StrikeEngine::Application* StrikeEngine::CreateApplication() 
{
	return new Sandbox();
}
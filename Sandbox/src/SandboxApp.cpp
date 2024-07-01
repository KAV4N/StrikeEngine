#include "strikepch.h"
#include <StrikeEngine.h>


class TestLayer : public StrikeEngine::Layer
{
public:
	TestLayer()
		: Layer("Test")
	{

	}
	void OnUpdate() override
	{
		STRIKE_INFO("Test_Layer_Update");
	}
	void OnEvent(StrikeEngine::Event& event) override
	{
		STRIKE_TRACE("{0}", event);
	}
};

class Sandbox : public StrikeEngine::Application 
{

public:
	Sandbox() 
	{
		PushLayer(new TestLayer());
		PushOverlay(new StrikeEngine::ImGuiLayer());
	}
	~Sandbox() 
	{

	}
};

StrikeEngine::Application* StrikeEngine::CreateApplication() 
{
	return new Sandbox();
}
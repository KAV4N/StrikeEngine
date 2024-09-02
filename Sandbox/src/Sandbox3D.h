#pragma once
#include <StrikeEngine.h>

class Sandbox3D : public StrikeEngine::Layer
{
public:
	Sandbox3D();

	virtual ~Sandbox3D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	virtual void OnImGuiRender() override;
	void OnEvent(StrikeEngine::Event& event) override;

private:
	StrikeEngine::World* m_World;
	float m_LastFrameTime;
};
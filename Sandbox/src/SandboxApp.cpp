#include "strikepch.h"
#include <StrikeEngine.h>

// ENTRY POINT ---------------------
#include "StrikeEngine/Core/EntryPoint.h"
//----------------------------------

#include "imgui.h"


class Sandbox : public StrikeEngine::Application 
{

public:
	Sandbox() 
	{
		auto* shaderManager = StrikeEngine::ShaderManager::Get();
		auto* materialManager = StrikeEngine::MaterialManager::Get();
		std::shared_ptr<StrikeEngine::Shader> testShader = shaderManager->LoadShader("assets/shaders/DefaultShader.glsl", "DefaultShader");
		std::shared_ptr<StrikeEngine::Material> testMaterial = materialManager->CreateMaterial(testShader, "DefaultMaterial");

		testMaterial->Set("u_Pos", 1);
		testMaterial->Set("u_Pos2", 1.f);
		testMaterial->Set("u_Pos2", 2.f);
		testMaterial->Bind();
	}
	~Sandbox() 
	{

	}
};

StrikeEngine::Application* StrikeEngine::CreateApplication() 
{
	return new Sandbox();
}
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
		auto* modelManager = StrikeEngine::ModelManager::Get();

		modelManager->LoadModel("assets/objects/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj", "panzer");

		std::shared_ptr<StrikeEngine::Shader> testShader = shaderManager->LoadShader("assets/shaders/DefaultShader.glsl", "DefaultShader");
		std::shared_ptr<StrikeEngine::Material> testMaterial = materialManager->CreateMaterial(testShader, "DefaultMaterial");

		

		testMaterial->SetUniform("u_Pos", 1);
		testMaterial->SetUniform("u_Pos2", 1.f);
		testMaterial->SetUniform("u_Pos2", 2.f);
		testMaterial->Apply();
	}
	~Sandbox() 
	{

	}
};

StrikeEngine::Application* StrikeEngine::CreateApplication() 
{
	return new Sandbox();
}
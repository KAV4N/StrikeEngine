#include "strikepch.h"
#include <StrikeEngine.h>

#include <pugixml.hpp>

// ENTRY POINT ---------------------
#include "StrikeEngine/Core/EntryPoint.h"
//----------------------------------

#include "imgui.h"

class Sandbox : public StrikeEngine::Application
{
public:
    Sandbox() {
        std::string modelPath_ = "assets/objects/panzer/tank_low_poly.fbx";
        std::string templateDir_ = "scenes/templates/tank";

        StrikeEngine::ModelParser parser;
        parser.parseModel("assets/objects/panzer/tank_low_poly.fbx", "scenes/templates/tank");
        parser.parseModel("assets/cubeModel.fbx", "scenes/templates/cubeModel");

        // Use Singleton instance of AssetManager
        StrikeEngine::AssetManager& manager = StrikeEngine::AssetManager::Get();

        manager.LoadAsset<StrikeEngine::Mesh>("mesh1", "scenes/templates/tank/Cube.mesh");
        auto mesh = manager.GetAsset<StrikeEngine::Mesh>("mesh1");

        auto& sceneManager = StrikeEngine::SceneManager::Get();
        sceneManager.SetScene("scenes/MainScene.xml");

    }

    ~Sandbox()
    {
    }
};

StrikeEngine::Application* StrikeEngine::CreateApplication()
{
    return new Sandbox();
}
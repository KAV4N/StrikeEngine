#include "strikepch.h"
#include <StrikeEngine.h>
#include "StrikeEngine/Core/EntryPoint.h"


namespace StrikeEngine {


    class Sandbox : public Application {
    public:
        Sandbox() {
            /*
            std::string modelPath = "assets/objects/panzer/tank_low_poly.fbx";
            std::string templateDir = "scenes/templates/tank";

            ModelParser parser;
            parser.parseModel("assets/objects/panzer/tank_low_poly.fbx", "scenes/templates/tank");
            parser.parseModel("assets/cubeModel.fbx", "scenes/templates/cubeModel");
            */
            /*
            ModelParser parser;
            parser.parseModel("Assets/objects/panzer/tank_low_poly.fbx", "Assets/scenes/templates/tank2");
            */
            //AssetManager& manager = AssetManager::get();
            //mesh = manager.loadMeshAsync("mesh1", "Assets/Scenes/templates/tank/Cube.mesh");
            //texture = manager.loadTexture("material1", "Assets/Objects/panzer/14077_WWII_Tank_Germany_Panzer_III_hull_diff.jpg");
            //material = manager.loadMaterialAsync("material1", "Assets/Scenes/templates/tank/Material.mat");
            //shader = manager.loadShader("shader3", "Assets/Shaders/testShader.vert", "Assets/Shaders/testShader.frag");
            
            auto& world = World::get();
            world.loadScene("Assets/Scenes/MainScene.xml");
            //AssetManager::get();
        }

        ~Sandbox() {
        }
        /*
        std::shared_ptr<StrikeEngine::Mesh> mesh;
        std::shared_ptr<StrikeEngine::Shader> shader;
        std::shared_ptr<StrikeEngine::Texture2D> texture;
        std::shared_ptr<StrikeEngine::Material> material;
        */
    };

    Application* createApplication() {
        return new Sandbox();
    }


}
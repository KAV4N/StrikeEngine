#include "strikepch.h"
#include <StrikeEngine.h>
#include "StrikeEngine/Core/EntryPoint.h"




namespace StrikeEngine {


    class Sandbox : public Application {
    public:
        Sandbox() {
            auto& world = World::get();
            world.loadScene("Assets/Scenes/MainScene.xml");
            /*
            AssetManager::get().update();

            world.onUpdate(.01);



            StrikeEngine::World::get().loadScene("Assets/Scenes/SecondScene.xml");
            */

            //auto data = audio->getData();
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
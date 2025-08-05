#include "strikepch.h"
#include <StrikeEngine.h>
#include <pugixml.hpp>
#include "StrikeEngine/Core/EntryPoint.h"
#include "imgui.h"

namespace StrikeEngine {

    class TestLayer : public Layer {
    public:
        TestLayer(std::shared_ptr<Mesh> mesh, const std::string& name = "TestLayer")
            : Layer(name), mMesh(mesh) {
        }

        void onUpdate(float deltaTime) override {
            if (mMesh) {
                if (mMesh->isReady()) {
                    STRIKE_INFO("Mesh '{}' is loaded and ready.", mMesh->getName());
                    auto asset = StrikeEngine::AssetManager::get().getAsset<Mesh>("mesh1");
                }
                else if (mMesh->isLoading()) {
                    STRIKE_INFO("Mesh '{}' is still loading.", mMesh->getName());
                }
                else if (mMesh->hasFailed()) {
                    STRIKE_ERROR("Mesh '{}' failed to load.", mMesh->getName());
                }
                else {
                    STRIKE_INFO("Mesh '{}' is uninitialized.", mMesh->getName());
                }
            }
            else {
                STRIKE_ERROR("No mesh assigned to TestLayer.");
            }
        }

    private:
        std::shared_ptr<Mesh> mMesh;
    };

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
            AssetManager& manager = AssetManager::get();
            auto mesh = manager.loadAssetAsync<Mesh>("mesh1", "Assets/Scenes/templates/tank/Cube.mesh");
            pushLayer(new TestLayer(mesh));
        }

        ~Sandbox() {
        }
    };

    Application* createApplication() {
        return new Sandbox();
    }
}
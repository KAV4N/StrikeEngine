#include "strikepch.h"


#include <StrikeEngine.h>
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Scene/World.h"


int main(int argc, char** argv) {
    StrikeEngine::Log::init();

    StrikeEngine::Application app;

    auto& world = StrikeEngine::World::get();
    world.loadScene("Assets/Scenes/SecondScene.xml");

    app.run();

    return 0;
}
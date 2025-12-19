#include "strikepch.h"
#define ENABLE_PROFILER

#include <StrikeEngine.h>
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Scene/World.h"


int main(int argc, char** argv) {
    StrikeEngine::Log::init();

    StrikeEngine::Application app;

    auto& world = StrikeEngine::World::get();
    world.loadScene("Assets/Scenes/MainScene.xml");

    app.run();

    return 0;
}
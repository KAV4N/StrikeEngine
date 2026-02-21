#include "strikepch.h"
#include <StrikeEngine.h>


int main(int argc, char** argv) {
    StrikeEngine::Application app;

    auto& world = StrikeEngine::World::get();
    world.loadScene("Assets/Scenes/HierarchyShowcase.xml");

    app.run();

    return 0;
}
#include "strikepch.h"
#include <StrikeEngine.h>


int main(int argc, char** argv) {
    Strike::Application app;

    auto& world = Strike::World::get();
    world.loadScene("Assets/Scenes/HierarchyShowcase.xml");

    app.run();

    return 0;
}
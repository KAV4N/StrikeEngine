#include <StrikeEngine.h>

int main(int argc, char** argv) {
    Strike::Application app;

    auto& world = Strike::World::get();
    world.loadScene("Assets/Scenes/game.xml");
    world.setGravity(glm::vec3(0,-150 ,0));

    app.run();
    return 0;
}
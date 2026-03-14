#include <StrikeEngine.h>

int main(int argc, char** argv) {
    Strike::Application app;
    //app.getWindow().setVSync(true);

    auto& world = Strike::World::get();
    world.loadScene("Assets/Scenes/game.xml");
    world.setGravity(glm::vec3(0,-150 ,0));

    app.run();
    return 0;
}
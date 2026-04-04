#include <StrikeEngine.h>

int main(int argc, char** argv) {
    try{
        Strike::Application app;
        //app.getWindow().setVSync(true);

        auto& world = Strike::World::get();
        world.loadScene("Assets/Scenes/lightTest.xml");
        world.setGravity(glm::vec3(0,-150 ,0));

        app.run();
    }catch (const std::runtime_error& e) {
        return 1;
    }

    return 0;
}
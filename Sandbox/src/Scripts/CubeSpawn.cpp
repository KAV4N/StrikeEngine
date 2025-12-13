#include "CubeSpawn.h"
#include "DeathTimer.h"
#include <glm/glm.hpp>

void CubeSpawn::onStart() {

}

void CubeSpawn::onUpdate(float deltaTime) {
    //if (StrikeEngine::Input::isMouseButtonPressed(STRIKE_MOUSE_BUTTON_3))  spawnCube();
        
}

void CubeSpawn::onMouseButtonPressed(StrikeEngine::MouseButtonPressedEvent& event){
    if (event.getMouseButton() == STRIKE_MOUSE_BUTTON_3){
        spawnCube();
    }

}

void CubeSpawn::spawnCube(){
    StrikeEngine::Scene* scene = getEntity().getScene();
    
    
    glm::mat4 myWorld = getEntity().getWorldMatrix();
    glm::vec3 forward = -glm::vec3(myWorld[2]);
    
    StrikeEngine::Entity entity = scene->createEntity();
    auto& renderer = entity.addComponent<StrikeEngine::RendererComponent>();


    entity.setPosition(getEntity().getPosition() + forward * 10.0f);
    renderer.setMesh("box", 0, "CCP2_CCP2");


    auto& physics = entity.addComponent<StrikeEngine::PhysicsComponent>();
    auto& logic = entity.addComponent<StrikeEngine::LogicComponent>();
    logic.addScript<DeathTimer>();


    counter++;
    std::cout << counter << std::endl;
    /*
    physics.setCanCollide(true);
    physics.setAnchored(false);

    physics.setMass(1.0f); // Make sure mass is set
    */
    float throwSpeed = 500.0f; 
    physics.setVelocity(forward * throwSpeed);
    physics.setAngularVelocity(glm::vec3(100,0,0));
}


REGISTER_SCRIPT(CubeSpawn);
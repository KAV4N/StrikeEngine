#include "CubeSpawn.h"
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
    
    auto& myTransform = getEntity().getComponent<StrikeEngine::TransformComponent>();
    
    glm::mat4 myWorld = myTransform.getWorldMatrix();
    glm::vec3 forward = -glm::vec3(myWorld[2]);
    
    StrikeEngine::Entity entity = scene->createEntity();
    auto& transform = entity.getComponent<StrikeEngine::TransformComponent>();
    auto& renderer = entity.addComponent<StrikeEngine::RendererComponent>();


    transform.setPosition(myTransform.getPosition() + forward * 10.0f);
    renderer.setMesh("box", 0, "CCP2_CCP2");


    auto& physics = entity.addComponent<StrikeEngine::PhysicsComponent>();


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
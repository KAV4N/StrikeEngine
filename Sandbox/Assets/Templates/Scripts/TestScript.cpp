#include "TestScript.h"
#include <iostream>

void TestScript::onUpdate(float deltaTime) {
    StrikeEngine::Entity entity = getEntity();
    entity.setEulerRotation(glm::vec3(rotate++ , rotate++,0));

    //std::cout << "x: " << entity.getEulerRotation()[0] << " y: " << entity.getEulerRotation()[1] << " z: " << entity.getEulerRotation()[2] << std::endl;
}

REGISTER_SCRIPT(TestScript);


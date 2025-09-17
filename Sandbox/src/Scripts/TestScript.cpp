#include "TestScript.h"
#include <iostream>

void TestScript::onUpdate(float deltaTime) {
    StrikeEngine::Entity entity = getEntity();
    entity.setRotation(glm::vec3(rotate++ , 0,0));
    std::cout << entity.getRotation()[0] << std::endl;
}

REGISTER_SCRIPT(TestScript);


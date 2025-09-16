#include "TestScript.h"
#include <iostream>

void TestScript::onUpdate(float deltaTime) {
    std::cout << "IM ALIVE\n";
}

REGISTER_SCRIPT(TestScript);


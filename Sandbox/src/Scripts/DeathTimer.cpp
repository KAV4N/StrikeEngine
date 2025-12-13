#include "DeathTimer.h"


void DeathTimer::onUpdate(float deltaTime) {
    if (tick(1.0f)) {
        std::cout << "DeathTimer: 1 second passed, destroying entity." << std::endl;
        //getEntity().destroy();
    }
        
}


REGISTER_SCRIPT(DeathTimer);
#include "CameraMovement.h"
#include <iostream>
#include <cstdlib>

void CameraMovement::onStart() {
    camera = getEntity();
}

void CameraMovement::onUpdate(float deltaTime) {
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_D)) {
        camera.moveRight(1 * deltaTime);
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_A)) {
        camera.moveLeft(1 * deltaTime);
    }

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_W)) {
        camera.moveForward(1 * deltaTime);
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_S)) {
        camera.moveBackward(1 * deltaTime);
    }

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_SPACE)) {
        camera.moveUp(1 * deltaTime);
    }

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) {
        camera.moveDown(1 * deltaTime);
    }

    if (StrikeEngine::Input::isMouseButtonPressed(STRIKE_MOUSE_BUTTON_1)) {
        auto& assetManager = StrikeEngine::AssetManager::get();
        auto sceneGraph = camera.getSceneGraph();
        idCounter++;
        std::stringstream ss;
        ss << "test" << idCounter;
        auto newEnt = sceneGraph->createEntity(ss.str());

        newEnt.setPositionX((std::rand() % 201) - 100);
        newEnt.setPositionY(-200);

        auto tankTemplate = assetManager.getTemplate("panzer");
        tankTemplate->instantiate(newEnt);
    }
}

void CameraMovement::onEvent(StrikeEngine::Event& event) {
    bindEvent<StrikeEngine::KeyReleasedEvent>(*this, &CameraMovement::keyEventTest);
}


bool CameraMovement::keyEventTest(StrikeEngine::KeyReleasedEvent& event) {

    STRIKE_CORE_INFO(event);

    return true;
}

REGISTER_SCRIPT(CameraMovement);
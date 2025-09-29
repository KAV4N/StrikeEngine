#include "CameraMovement.h"

void CameraMovement::onStart() {
    mCameraEntity = getEntity();
}

void CameraMovement::printStats(const std::string& state) {
    std::cout << "---- " << state << " ----" << std::endl;

    glm::vec3 pos = mCameraEntity.getPosition();
    std::cout << "Position: x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;

    glm::vec3 euler = mCameraEntity.getEulerRotation();
    std::cout << "Euler:    x=" << euler.x << ", y=" << euler.y << ", z=" << euler.z << std::endl;

    glm::quat rot = mCameraEntity.getRotation();
    std::cout << "Quat:     x=" << rot.x << ", y=" << rot.y << ", z=" << rot.z << ", w=" << rot.w << std::endl;

    std::cout << std::endl;
}


void CameraMovement::onUpdate(float deltaTime) {
    // Movement controls
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_D)) {
        mCameraEntity.moveRight(20 * deltaTime);
        printStats("move RIGHT:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_A)) {
        mCameraEntity.moveLeft(20 * deltaTime);
        printStats("move LEFT:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_W)) {
        mCameraEntity.moveForward(20 * deltaTime);
        printStats("move FORW:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_S)) {
        mCameraEntity.moveBackward(20 * deltaTime);
        printStats("move BACK:");
    }

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_SPACE)) {
        mCameraEntity.moveUp(20 * deltaTime); 
        printStats("move UP:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) {
        mCameraEntity.moveDown(20 * deltaTime);
        printStats("move DOWN:");
    }

    // Rotation controls
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT)) {
        mCameraEntity.rotateY(90.0f * deltaTime); 
        printStats("rotate +Y:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_RIGHT)) {
        mCameraEntity.rotateY(-90.0f * deltaTime);
        printStats("rotate -Y:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_UP)) {
        mCameraEntity.rotateX(90.0f * deltaTime);
        printStats("rotate -X:");
    }
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_DOWN)) {
        mCameraEntity.rotateX(-90.0f * deltaTime); 
        printStats("rotate +X:"); 
    }
    /*
    if (StrikeEngine::Input::isMouseButtonPressed(STRIKE_MOUSE_BUTTON_1)) {
        auto& assetManager = StrikeEngine::AssetManager::get();
        auto sceneGraph = mCameraEntity.getSceneGraph();
        idCounter++;

        std::stringstream ss;
        ss << "test" << idCounter;
        auto newEnt = sceneGraph->createEntity(ss.str());

        newEnt.setPosition(glm::vec3(std::rand() % 201- 100, -200, 0.f));

        auto tankTemplate = assetManager.getTemplate("panzer");
        if (tankTemplate)
            tankTemplate->instantiate(newEnt);
    }*/
}

void CameraMovement::onEvent(StrikeEngine::Event& event) {
    bindEvent<StrikeEngine::KeyReleasedEvent>(*this, &CameraMovement::keyEventTest);
}

bool CameraMovement::keyEventTest(StrikeEngine::KeyReleasedEvent& event) {

    return true;
}

REGISTER_SCRIPT(CameraMovement);

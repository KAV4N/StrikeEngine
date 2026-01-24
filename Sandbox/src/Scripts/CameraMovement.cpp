#include "CameraMovement.h"

void CameraMovement::onStart() {
    mCameraEntity = getEntity();
}

void CameraMovement::printStats(const std::string& state) {
    std::cout << "---- " << state << " ----" << std::endl;

    glm::vec3 pos = mCameraEntity.getPosition();
    std::cout << "Position: x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;

    glm::vec3 euler = mCameraEntity.getEulerAngles();
    std::cout << "Euler:    x=" << euler.x << ", y=" << euler.y << ", z=" << euler.z << std::endl;

    glm::quat rot = mCameraEntity.getRotation();
    std::cout << "Quat:     x=" << rot.x << ", y=" << rot.y << ", z=" << rot.z << ", w=" << rot.w << std::endl;

    std::cout << std::endl;
}

void CameraMovement::onUpdate(float deltaTime)
{

    auto scene = getEntity().getScene();
    
    StrikeEngine::Application::get().getWindow().setWindowTitle(
        "StrikeEngine - Scene: " + scene->getId() +
        " | FPS: " + std::to_string(static_cast<int>(StrikeEngine::Application::get().getCurrentFPS()))
    );

    


    // --------------------------------------------------------------------
    // 1. Movement (local space)
    // --------------------------------------------------------------------
    glm::vec3 move(0.0f);
    const float speed = 20.0f * deltaTime;

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_D)) move.x += speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_A)) move.x -= speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_W)) move.z -= speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_S)) move.z += speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_SPACE)) move.y += speed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) move.y -= speed;

    if (glm::length(move) > 0.0f) {
        mCameraEntity.move(move);
        printStats("After Move");
    }

    // --------------------------------------------------------------------
    // 2. Rotation
    // --------------------------------------------------------------------
    glm::vec3 rotation(0.0f);
    const float rotSpeed = 90.0f * deltaTime;

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT))  rotation.y += rotSpeed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_RIGHT)) rotation.y -= rotSpeed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_UP))    rotation.x += rotSpeed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_DOWN))  rotation.x -= rotSpeed;

    if (glm::length(rotation) > 0.0f)
        mCameraEntity.rotate(rotation);

    // --------------------------------------------------------------------
    // 3. Raycast test
    // --------------------------------------------------------------------
    {
        glm::vec3 camPos = mCameraEntity.getPosition();
        glm::vec3 camForward = mCameraEntity.getForward();

        const float maxDist = 500.0f;

        StrikeEngine::RayHit hit =
            StrikeEngine::World::get().rayCast(camPos, camForward, maxDist);

        if (hit.hasHit()) {
            std::string tag = hit.entity.getTag();
            if (tag.empty()) tag = "<no tag>";

            std::cout << "HIT -> Tag: \"" << tag
                      << "\" | Distance: " << hit.distance
                      << " | Normal: (" << hit.normal.x
                      << ", " << hit.normal.y
                      << ", " << hit.normal.z << ")"
                      << std::endl;
        }
    }

    // --------------------------------------------------------------------
    // 4. Collision debug
    // --------------------------------------------------------------------
    auto collidingEntities = mCameraEntity.getCollidingEntities();
    if (!collidingEntities.empty()) {
        std::cout << "Camera is colliding with "
                  << collidingEntities.size() << " entities:" << std::endl;
        for (const auto& ent : collidingEntities) {
            std::string tag = ent.getTag();
            if (tag.empty()) tag = "<no tag>";
            std::cout << "  - Tag: \"" << tag << "\"" << std::endl;
        }
    }


}

void CameraMovement::onEvent(StrikeEngine::Event& event) {
    if (event.getEventType() == StrikeEngine::EventType::MouseButtonPressed) {
        auto& mouseEvent = static_cast<StrikeEngine::MouseButtonPressedEvent&>(event);
        if (mouseEvent.getMouseButton() == STRIKE_MOUSE_BUTTON_LEFT) {
            spawnLight();
        } else if (mouseEvent.getMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
            swapScene();
        }
        event.handled = true;
    }
}

void CameraMovement::spawnLight() {
    auto scene = mCameraEntity.getScene();
    auto newEnt = scene->createEntity();

    newEnt.setPosition(mCameraEntity.getPosition());
    auto& pointLight = newEnt.addComponent<StrikeEngine::LightComponent>();

    float r = static_cast<float>(rand() % 256);
    float g = static_cast<float>(rand() % 256);
    float b = static_cast<float>(rand() % 256);

    pointLight.setColor(glm::vec3(r, g, b));
    pointLight.setRadius(5);
    pointLight.setIntensity(5);
}

void CameraMovement::swapScene() {
    auto scene = StrikeEngine::World::get().getScene();
    if (scene){
        if (scene->getId() == "SecondScene") {
             StrikeEngine::World::get().loadScene("Assets/Scenes/MainScene.xml");
        } else {
             StrikeEngine::World::get().loadScene("Assets/Scenes/SecondScene.xml");
        }
    }
}

REGISTER_SCRIPT(CameraMovement);

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

void CameraMovement::onUpdate(float deltaTime)
{
    // --------------------------------------------------------------------
    // 1. Gather input → speed per axis (local space)
    // --------------------------------------------------------------------
    glm::vec3 move(0.0f);               // local-space movement (right, up, forward)
    const float speed = 20.0f * deltaTime;

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_D))      move.x += speed;   // right
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_A))      move.x -= speed;   // left
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_W))      move.z -= speed;   // forward
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_S))      move.z += speed;   // back
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_SPACE))  move.y += speed;   // up
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) move.y -= speed; // down

    // --------------------------------------------------------------------
    // 2. Transform local movement into world space
    // --------------------------------------------------------------------
    if (glm::length(move) > 0.0f)
    {
        const glm::quat rot = mCameraEntity.getRotation();

        // local axes in world space
        const glm::vec3 right   = rot * glm::vec3(1,0,0);
        const glm::vec3 up      = rot * glm::vec3(0,1,0);
        const glm::vec3 forward = rot * glm::vec3(0,0,1);   

        const glm::vec3 worldMove = right * move.x + up * move.y + forward * move.z;
        mCameraEntity.move(worldMove);
    }

    // --------------------------------------------------------------------
    // 3. Rotation
    // --------------------------------------------------------------------
    glm::vec3 rotation(0.0f);
    const float rotSpeed = 90.0f * deltaTime;   // degrees per second

    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_LEFT))   rotation.y += rotSpeed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_RIGHT))  rotation.y -= rotSpeed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_UP))     rotation.x += rotSpeed;
    if (StrikeEngine::Input::isKeyPressed(STRIKE_KEY_DOWN))   rotation.x -= rotSpeed;

    if (glm::length(rotation) > 0.0f)
        mCameraEntity.rotateEuler(rotation);

    // --------------------------------------------------------------------
    // 4. Raycast test – print hit entity tag
    // --------------------------------------------------------------------
    {
        glm::vec3 camPos = mCameraEntity.getPosition();
        glm::quat camRot = mCameraEntity.getRotation();

        // Forward direction in world space (-Z local)
        glm::vec3 forwardDir = camRot * glm::vec3(0.0f, 0.0f, -1.0f);

        // Raycast far enough to be useful for testing
        const float maxDist = 500.0f;

        StrikeEngine::RayHit hit = StrikeEngine::World::get().rayCast(camPos, forwardDir, maxDist);

        if (hit.hasHit()) {
            std::string tag = hit.entity.getTag();
            if (tag.empty()) tag = "<no tag>";

            std::cout << "HIT -> Tag: \"" << tag
                      << "\" | Distance: " << hit.distance
                      << " | Normal: (" << hit.normal.x << ", " << hit.normal.y << ", " << hit.normal.z << ")"
                      << std::endl;
        } 
    }

    // --------------------------------------------------------------------
    // 5. Existing collision debug (kept for reference)
    // --------------------------------------------------------------------
    std::vector<StrikeEngine::Entity> collidingEntities = mCameraEntity.getCollidingEntities();
    if (!collidingEntities.empty()) {
        std::cout << "Camera is colliding with " << collidingEntities.size() << " entities:" << std::endl;
        for (const auto& ent : collidingEntities) {
            std::string tag = ent.getTag();
            if (tag.empty()) tag = "<no tag>";
            std::cout << "  - Tag: \"" << tag << "\"" << std::endl;
        }
    }
}

void CameraMovement::onMouseButtonPressed(StrikeEngine::MouseButtonPressedEvent& event){
    if (event.getMouseButton() == STRIKE_MOUSE_BUTTON_1){
        swapScene();
    }
    if (event.getMouseButton() == STRIKE_MOUSE_BUTTON_2){
        spawnLight();
    }
}

void CameraMovement::spawnLight(){
    auto scene = mCameraEntity.getScene();
    auto newEnt = scene->createEntity();
    
    newEnt.setPosition(mCameraEntity.getPosition());
    auto& pointLight = newEnt.addComponent<StrikeEngine::LightComponent>();
    pointLight.setFallOff(25);

    // Random color (0-255 for each channel)
    float r = static_cast<float>(rand() % 256);
    float g = static_cast<float>(rand() % 256);
    float b = static_cast<float>(rand() % 256);

    pointLight.setColor(glm::vec3(r, g, b));

    pointLight.setRadius(100);
    pointLight.setIntensity(20);
}

void CameraMovement::swapScene() {
    auto& world = StrikeEngine::World::get();
    auto scene = world.getScene();

    if (scene->getId() == "SecondScene"){
        StrikeEngine::World::get().loadSceneAsync("Assets/Scenes/MainScene.xml");
    }else{
        StrikeEngine::World::get().loadSceneAsync("Assets/Scenes/SecondScene.xml");
    }
}

REGISTER_SCRIPT(CameraMovement);
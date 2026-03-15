#include "PlayerController.h"

void PlayerController::loadFootstepSounds()
{
    auto& am = Strike::AssetManager::get();

    auto  scene  = getEntity().getScene();
    auto  player = getEntity();

    mFootstepEntityA = scene->createEntity();
    mFootstepEntityA.setTag("FootstepA");
    mFootstepEntityA.setParent(player);
    {
        auto& src = mFootstepEntityA.addComponent<Strike::AudioSourceComponent>();
        src.setAudio(kFootstepAudioIdA);
        src.setVolume(0.6f);
        src.setLoop(false);
        src.setSpatial(false);
    }

    mFootstepEntityB = scene->createEntity();
    mFootstepEntityB.setTag("FootstepB");
    mFootstepEntityB.setParent(player);
    {
        auto& src = mFootstepEntityB.addComponent<Strike::AudioSourceComponent>();
        src.setAudio(kFootstepAudioIdB);
        src.setVolume(0.6f);
        src.setLoop(false);
        src.setSpatial(false);
    }
}

void PlayerController::updateFootsteps(bool isMoving, bool isSprinting)
{
    if (!isMoving || !mIsGrounded) {
        mWasMoving = false;
        return;
    }

    float interval = isSprinting ? kSprintStepInterval : kWalkStepInterval;

    if (tick(interval)) {
        Strike::Entity& current = mNextIsA ? mFootstepEntityA : mFootstepEntityB;
        mNextIsA = !mNextIsA;

        if (current.isValid() && current.hasComponent<Strike::AudioSourceComponent>())
            current.getComponent<Strike::AudioSourceComponent>().play();
    }

    mWasMoving = true;
}

void PlayerController::onStart()
{
    Strike::Input::setCursorMode(Strike::CursorMode::Locked);

    mCameraEntity = getEntity().getScene()->getEntity("Camera");
    if (!mCameraEntity.isValid())
        STRIKE_CORE_WARN("PlayerController: could not find child entity tagged 'Camera'");

    if (mCameraEntity.isValid())
        mPitch = mCameraEntity.getEulerAngles().x;

    loadFootstepSounds();
}

bool PlayerController::isGrounded()
{
    glm::vec3 origin = scriptEntity.getWorldPosition();
    float dist = mPlayerHalfH + mGroundRayExt;
    auto hit = Strike::World::get().rayCast(origin, glm::vec3(0.0f, -1.0f, 0.0f), dist);
    return hit.hasHit() && hit.entity != getEntity();
}

void PlayerController::onUpdate(float deltaTime)
{
    Strike::Application::get().getWindow()
        .setWindowTitle("FPS: " + std::to_string(
            static_cast<int>(Strike::Application::get().getCurrentFPS())));

    mIsGrounded = isGrounded();

    auto& physics = getComponent<Strike::PhysicsComponent>();

    auto [dX, dY] = Strike::Input::getMouseDelta();

    if (glm::abs(dX) > 0.0001f)
    {
      
        float yawSpeed = (-dX * mSensitivity) / deltaTime;
        physics.setAngularVelocity(glm::vec3(0.0f, glm::radians(yawSpeed), 0.0f));
    }
    else
    {
        physics.setAngularVelocity(glm::vec3(0.0f));
    }

    if (mCameraEntity.isValid() && glm::abs(dY) > 0.0001f)
    {
        mPitch = glm::clamp(mPitch + (-dY * mSensitivity), -mPitchLimit, mPitchLimit);
        mCameraEntity.setEulerAngles(glm::vec3(mPitch, 0.0f, 0.0f));
    }

    bool isSprinting = Strike::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT);
    float speed = mMoveSpeed * (isSprinting ? mSprintMult : 1.0f);

    glm::quat yawRot  = scriptEntity.getRotation();
    glm::vec3 forward = yawRot * glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right   = yawRot * glm::vec3(1.0f, 0.0f,  0.0f);
    forward.y = 0.0f;
    right.y   = 0.0f;
    if (glm::length(forward) > 0.001f) forward = glm::normalize(forward);
    if (glm::length(right)   > 0.001f) right   = glm::normalize(right);

    glm::vec3 moveDir(0.0f);
    if (Strike::Input::isKeyPressed(STRIKE_KEY_W)) moveDir += forward;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_S)) moveDir -= forward;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_D)) moveDir += right;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_A)) moveDir -= right;

    bool isMoving = glm::length(moveDir) > 0.001f;
    if (isMoving) moveDir = glm::normalize(moveDir);

    glm::vec3 currentVel = physics.getVelocity();
    glm::vec3 targetVel  = moveDir * speed;
    targetVel.y = currentVel.y;

    physics.setVelocity(targetVel);

    updateFootsteps(isMoving, isSprinting);
}

void PlayerController::onEvent(Strike::Event& e)
{
    if (e.getEventType() != Strike::EventType::KeyPressed) return;

    auto& key = static_cast<Strike::KeyPressedEvent&>(e);
    if (key.getRepeatCount() > 0) return;

    if (key.getKeyCode() == STRIKE_KEY_SPACE && mIsGrounded)
    {
        auto& physics = getComponent<Strike::PhysicsComponent>();
        glm::vec3 vel = physics.getVelocity();
        vel.y = mJumpImpulse;
        physics.setVelocity(vel);
        e.handled = true;
    }

    if (key.getKeyCode() == STRIKE_KEY_ESCAPE)
    {
        Strike::Input::setCursorMode(Strike::CursorMode::Normal);
        e.handled = true;
    }

    if (key.getKeyCode() == STRIKE_KEY_TAB)
    {
        Strike::Input::setCursorMode(Strike::CursorMode::Locked);
        e.handled = true;
    }
}

REGISTER_SCRIPT(PlayerController);

#include "strikepch.h"
#include "Script.h"
#include "StrikeEngine/Events/Event.h"
#include "Entity.h"

namespace Strike {

    void Script::onStart() {}
    void Script::onUpdate(float deltaTime) {}
    void Script::onDestroy() {}
    void Script::onEvent(Event& event) {}

    void Script::internalUpdate(float deltaTime) {
        for (auto& [interval, timer] : mTimers) {
            timer += deltaTime;
        }
        onUpdate(deltaTime);
    }

    Entity Script::getEntity() const {
        return mEntity;
    }

    bool Script::isActive() const {
        return mActive;
    }

    void Script::setActive(bool active) {
        mActive = active;
    }

    bool Script::isStarted() const {
        return mStarted;
    }

    void Script::setEntity(const Entity& entity) {
        if (!mStarted && entity.isValid()) {
            mEntity = entity;
        }
    }

    void Script::markStarted() {
        mStarted = true;
    }

    bool Script::tick(float seconds) {
        auto& timer = mTimers[seconds];
        if (timer >= seconds) {
            timer = 0.0f;
            return true;
        }
        return false;
    }
}
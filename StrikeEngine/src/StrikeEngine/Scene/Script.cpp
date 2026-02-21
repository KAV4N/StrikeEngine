#include "strikepch.h"
#include "Script.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/KeyEvent.h"
#include "StrikeEngine/Events/MouseEvent.h"
#include "Entity.h"

namespace Strike {

    Script::Script() = default;

    Script::~Script() {
        onDestroy();
    }

    void Script::onCreate() {}
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
    

    const Entity& Script::getEntity() const {
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
        if (mStarted == false && entity.isValid()) {
            mEntity = entity;
        }
    }

    void Script::markStarted() {
        mStarted = true;
    }

    bool Script::tick(float seconds) {
        // Get or create timer for this interval
        auto& timer = mTimers[seconds];
        
        // Check if the interval has passed
        if (timer >= seconds) {
            timer = 0.0f;  // Reset the timer
            return true;
        }
        
        return false;
    }

}
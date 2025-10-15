#include "Script.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/UserEvent.h"
#include "StrikeEngine/Core/Application.h"
#include "Entity.h"

namespace StrikeEngine {

    Script::Script() = default;

    Script::~Script() {
        onDestroy();
    }

    void Script::onCreate() {}
    void Script::onStart() {}
    void Script::onUpdate(float deltaTime) {}
    void Script::onDestroy() {}
    void Script::onEvent(Event& event) {  }

    void Script::sendUserEvent(const std::string& eventName, std::any data) {
        UserEvent event(eventName, data);
        Application::get().onEvent(event);
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

    void Script::setEventDispatcher(EventDispatcher* dispatcher) {
        mEventDispatcher = dispatcher;
    }
}
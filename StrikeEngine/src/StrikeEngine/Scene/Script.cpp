#include "Script.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/KeyEvent.h"
#include "StrikeEngine/Events/MouseEvent.h"
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
    
    void Script::onEvent(Event& event) {
        switch (event.getEventType()) {
            case EventType::KeyPressed:
                onKeyPressed(static_cast<KeyPressedEvent&>(event));
                break;
            case EventType::KeyReleased:
                onKeyReleased(static_cast<KeyReleasedEvent&>(event));
                break;
            case EventType::KeyTyped:
                onKeyTyped(static_cast<KeyTypedEvent&>(event));
                break;
            case EventType::MouseMoved:
                onMouseMoved(static_cast<MouseMovedEvent&>(event));
                break;
            case EventType::MouseScrolled:
                onMouseScrolled(static_cast<MouseScrolledEvent&>(event));
                break;
            case EventType::MouseButtonPressed:
                onMouseButtonPressed(static_cast<MouseButtonPressedEvent&>(event));
                break;
            case EventType::MouseButtonReleased:
                onMouseButtonReleased(static_cast<MouseButtonReleasedEvent&>(event));
                break;
            default:
                break;
        }
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

}
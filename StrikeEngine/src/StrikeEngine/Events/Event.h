#pragma once

#include "strikepch.h"
#include "StrikeEngine/Core/Core.h"


namespace StrikeEngine {

    enum class CursorMode {
        Normal,
        Locked,
        Hidden
    };

    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };


#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
                               virtual EventType getEventType() const override { return getStaticType(); }\
                               virtual const char* getName() const override { return #type; }


    class Event {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;
        virtual EventType getEventType() const = 0;
        virtual const char* getName() const = 0;
        virtual std::string toString() const { return getName(); }

    public:
        bool handled = false;
    };

    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
    public:
        EventDispatcher(Event& event) : mEvent(event) {}

        template<typename T>
        bool dispatch(EventFn<T> func) {
            if (mEvent.getEventType() == T::getStaticType()) {
                mEvent.handled = func(*(T*)&mEvent);
                return true;
            }
            return false;
        }

    private:
        Event& mEvent;
    };

    inline std::string format_as(const Event& e) {
        return e.toString();
    }


}
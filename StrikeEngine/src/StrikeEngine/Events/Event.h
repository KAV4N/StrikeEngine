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
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
        User
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4),
        EventCategoryUser = BIT(5) 
    };

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
                               virtual EventType getEventType() const override { return getStaticType(); }\
                               virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

    class Event {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;
        virtual EventType getEventType() const = 0;
        virtual const char* getName() const = 0;
        virtual int getCategoryFlags() const = 0;
        virtual std::string toString() const { return getName(); }
        virtual Event* clone() const = 0;

        bool isInCategory(EventCategory category) {
            return getCategoryFlags() & category;
        }

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
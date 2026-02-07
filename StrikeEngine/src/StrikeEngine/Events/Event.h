#pragma once

#include "strikepch.h"
#include "StrikeEngine/Core/Core.h"


namespace StrikeEngine {

    /**
     * @brief Enumeration of event types.
     */
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };


#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
                               virtual EventType getEventType() const override { return getStaticType(); }\
                               virtual const char* getName() const override { return #type; }

    /**
     * @brief Base class for all events in StrikeEngine.
     */
    class Event {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;

        /**
         * @brief Get the type of the event.
         * @return EventType enumeration value.
         */
        virtual EventType getEventType() const = 0;

        /**
         * @brief Get the name of the event.
         * @return Name of the event as a C-string.
         */
        virtual const char* getName() const = 0;

        /**
         * @brief Convert the event to a string representation.
         * @return String representation of the event.
         */
        virtual std::string toString() const { return getName(); }

    public:
        bool handled = false;
    };

    /**
     * @brief Class for dispatching events to appropriate handlers.
     * 
     */
    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
    public:
        EventDispatcher(Event& event) : mEvent(event) {}

        /**
         * @brief Dispatch the event to the appropriate handler if the types match.
         * @tparam T The specific event type to dispatch.
         * @param func The function to handle the event.
         * @return True if the event was dispatched and handled, false otherwise.
         */
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
    /**
     * @brief Format an event as a string.
     * @param e The event to format.
     */
    inline std::string format_as(const Event& e) {
        return e.toString();
    }


}
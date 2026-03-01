#pragma once

#include "strikepch.h"
#include "StrikeEngine/Core/Core.h"


namespace Strike {

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
     * @brief Format an event as a string.
     * @param e The event to format.
     */
    inline std::string format_as(const Event& e) {
        return e.toString();
    }


}
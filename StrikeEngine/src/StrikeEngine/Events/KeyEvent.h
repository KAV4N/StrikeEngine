#pragma once

#include "Event.h"

namespace StrikeEngine {

    class KeyEvent : public Event {
    public:

        /**
         * @brief Get the key code associated with the event.
         * @return Key code.
         */
        int getKeyCode() const { return mKeyCode; }

    protected:
        KeyEvent(int keycode) : mKeyCode(keycode) {}

        int mKeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keycode, int repeatCount)
            : KeyEvent(keycode), mRepeatCount(repeatCount) {
        }
        /**
         * @brief Get the repeat count of the key press event.
         * @return Number of times the key has been repeated.
         */
        int getRepeatCount() const { return mRepeatCount; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
          

    private:
        int mRepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}
        
        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
       
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
           
    };
}
#pragma once

#include "Event.h"

namespace StrikeEngine {

    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float x, float y) : mMouseX(x), mMouseY(y) {}

        float getX() const { return mMouseX; }
        float getY() const { return mMouseY; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)


    private:
        float mMouseX, mMouseY;
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float xOffset, float yOffset) : mXOffset(xOffset), mYOffset(yOffset) {}

        float getXOffset() const { return mXOffset; }
        float getYOffset() const { return mYOffset; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)

    private:
        float mXOffset, mYOffset;
    };

    class MouseButtonEvent : public Event {
    public:
        int getMouseButton() const { return mButton; }

    protected:
        MouseButtonEvent(int button) : mButton(button) {}

        int mButton;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << mButton;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)

    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << mButton;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
            
    };
}
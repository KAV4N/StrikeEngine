#pragma once

#include "Event.h"

namespace Strike {
    /**
     * @brief Event class for mouse movement events.
     */
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float x, float y) : mMouseX(x), mMouseY(y) {}
        /**
         * @brief Get the X position of the mouse.
         * @return Mouse X position.
         */
        float getX() const { return mMouseX; }

        /**
         * @brief Get the Y position of the mouse.
         *  @return Mouse Y position.
         */
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

    /**
     * @brief Event class for mouse scroll events.
     */
    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float xOffset, float yOffset) : mXOffset(xOffset), mYOffset(yOffset) {}

        /**
         * @brief Get the horizontal scroll offset.
         * @return Horizontal scroll offset.
         */
        float getXOffset() const { return mXOffset; }

        /**
         * @brief Get the vertical scroll offset.
         * @return Vertical scroll offset.
         */
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

    /**
     * @brief Base class for mouse button events.
     */
    class MouseButtonEvent : public Event {
    public:

        /**
         * @brief Get the mouse button associated with the event.
         * @return Mouse button code.
         */
        int getMouseButton() const { return mButton; }

    protected:
        MouseButtonEvent(int button) : mButton(button) {}

        int mButton;
    };

    /**
     * @brief Event class for mouse button press events.
     */
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

    /**
     * @brief Event class for mouse button release events.
     */

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
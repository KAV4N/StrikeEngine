#pragma once

#include "Event.h"

namespace Strike {

    /**
     * @brief Event class for window resize events.
     */
    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : mWidth(width), mHeight(height) {
        }

        unsigned int getWidth() const { return mWidth; }
        unsigned int getHeight() const { return mHeight; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)

    private:
        unsigned int mWidth, mHeight;
    };

    /**
     * @brief Event class for window close events.
     */
    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)

    };
}
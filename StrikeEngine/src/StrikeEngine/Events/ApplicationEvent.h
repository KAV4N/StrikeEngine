#pragma once

#include "Event.h"

namespace StrikeEngine {

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
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

            Event* clone() const override { return new WindowResizeEvent(mWidth, mHeight); }

    private:
        unsigned int mWidth, mHeight;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

            Event* clone() const override { return new WindowCloseEvent(); }
    };

    class AppTickEvent : public Event {
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(AppTick)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

            Event* clone() const override { return new AppTickEvent(); }
    };

    class AppUpdateEvent : public Event {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(AppUpdate)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

            Event* clone() const override { return new AppUpdateEvent(); }
    };

    class AppRenderEvent : public Event {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(AppRender)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

            Event* clone() const override { return new AppRenderEvent(); }
    };
}

#include "Event.h"
#include <any>

namespace StrikeEngine {
    class UserEvent : public Event {
    public:
        UserEvent(const std::string& eventName, std::any data = {})
            : mEventName(eventName), mData(std::move(data)) {
        }

        const std::string& getEventName() const { return mEventName; }
        const std::any& getData() const { return mData; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "UserEvent: " << mEventName;
            return ss.str();
        }

        EVENT_CLASS_TYPE(User)
        EVENT_CLASS_CATEGORY(EventCategoryUser)

        Event* clone() const override {
            return new UserEvent(mEventName, mData);
        }

    private:
        std::string mEventName;
        std::any mData;
    };
}

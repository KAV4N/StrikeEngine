#pragma once

#include <queue>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include "Event.h"
#include "ApplicationEvent.h"
#include "StrikeEngine/Core/Application.h"

namespace StrikeEngine
{
    class EventBus
    {
    public:
        // Creates and initializes the EventBus instance
        static void Create(LayerStack& ls);

        // Returns the singleton instance of the EventBus
        static EventBus* Get();

        // Pushes an event onto the event queue
        void PushEvent(Event* event);

        // Processes all events in the queue
        void ProcessEvents();

    private:
        EventBus(LayerStack& ls);
        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;

        // Dispatches an event to the appropriate handlers
        void DispatchEvent(Event& e);

        static EventBus* s_Instance;
        LayerStack& m_LayerStack;
        std::queue<Event*> m_EventQueue;
    };
}

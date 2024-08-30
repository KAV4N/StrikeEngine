#include "strikepch.h"
#include "EventBus.h"

namespace StrikeEngine
{
    EventBus* EventBus::s_Instance = nullptr;

    void EventBus::Create(LayerStack& ls)
    {
        if (s_Instance == nullptr)
            s_Instance = new EventBus(ls);
    }

    EventBus* EventBus::Get()
    {
        return s_Instance;
    }

    EventBus::EventBus(LayerStack& ls)
        : m_LayerStack(ls) 
    {
    }

    void EventBus::PushEvent(Event* event)
    {
        m_EventQueue.push(event);
    }

    void EventBus::ProcessEvents()
    {
        while (!m_EventQueue.empty())
        {
            Event* event = m_EventQueue.front();
            m_EventQueue.pop();

            DispatchEvent(*event);

            delete event;
        }
    }

    void EventBus::DispatchEvent(Event& e)
    {
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }
}

#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

    class Layer {
    public:
        Layer(const std::string& name = "Layer", const bool active=true);
        virtual ~Layer();

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(float deltaTime) {}
        virtual void onImGuiRender() {}
        virtual void onRender() {}
        virtual void onEvent(Event& event) {}

        inline const std::string& getName() const { return mName; }

        bool IsActive() { return mActive; }
        void setActive(bool active) { mActive = active; }

    protected:
        std::string mName;
        bool mActive;
    };
}
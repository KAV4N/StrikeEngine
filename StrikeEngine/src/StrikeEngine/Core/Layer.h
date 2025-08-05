#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

    class Layer {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(float deltaTime) {}
        virtual void onImGuiRender() {}
        virtual void onRender() {}
        virtual void onEvent(Event& event) {}

        inline const std::string& getName() const { return mDebugName; }

    protected:
        std::string mDebugName;
    };
}
#include "strikepch.h"
#include "Layer.h"

namespace StrikeEngine {
    Layer::Layer(const std::string& name, const bool active)
        : mName(name) ,
        mActive(active)
    {
    }

    Layer::~Layer() {
    }
}
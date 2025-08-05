#include "strikepch.h"
#include "Layer.h"

namespace StrikeEngine {
    Layer::Layer(const std::string& debugName)
        : mDebugName(debugName) {
    }

    Layer::~Layer() {
    }
}
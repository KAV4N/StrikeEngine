#pragma once
#include <string>

#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include <glad/glad.h>

namespace StrikeEngine {

    class RenderPass {
    public:
        RenderPass(const std::string& name) : mName(name), mEnabled(true) {}
        virtual ~RenderPass() = default;

        virtual void execute(const CameraRenderData& cameraData) = 0;

        void setEnabled(bool enabled) { mEnabled = enabled; }
        bool isEnabled() const { return mEnabled; }

        const std::string& getName() const { return mName; }

        virtual void setup() {}
        virtual void cleanup() {}

    protected:
        std::string mName;
        bool mEnabled;
    };

}
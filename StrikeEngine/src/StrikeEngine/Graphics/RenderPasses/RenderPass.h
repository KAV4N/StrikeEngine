#pragma once
#include <string>

#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include <glad/glad.h>

namespace StrikeEngine {

    class RenderPass {
    public:
        RenderPass(const std::string& name) : mName(name) {}
        virtual ~RenderPass() = default;

        virtual void execute(const CameraRenderData& cameraData) = 0;

        const std::string& getName() const { return mName; }

        virtual void setup() {}
        virtual void cleanup() {}

    protected:
        std::string mName;
    };

}
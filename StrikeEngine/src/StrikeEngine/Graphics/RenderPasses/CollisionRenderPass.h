#pragma once
#include "RenderPass.h"
#include <memory>
#include <glm/glm.hpp>


namespace StrikeEngine {

    class Shader;
    class Renderer;

    class CollisionRenderPass : public RenderPass {
    public:
        CollisionRenderPass(Renderer& renderer);
        ~CollisionRenderPass() override;

        void setup() override;
        void cleanup() override;
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();
        void setupBoxGeometry();
        void renderBox(const glm::mat4& modelMatrix, const glm::mat4& viewProj);

        Renderer& mRenderer;
        std::shared_ptr<Shader> mShader;
        GLuint mBoxVAO, mBoxVBO, mBoxEBO;
        uint32_t mBoxIndexCount;
    };

}
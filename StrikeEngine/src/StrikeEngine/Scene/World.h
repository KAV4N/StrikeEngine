#pragma once

#include <vector>
#include <memory>
#include "Scene.h"
#include <StrikeEngine/Events/Event.h>
#include "StrikeEngine/Core/Layer.h"

namespace StrikeEngine {

    class World : Layer{
    public:
        static void Create();
        static World* Get();
        static Scene* GetActiveScene();


        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnEvent(Event& event) override;

        void AddScene(Scene* scene);
        void AddScene();
        void SetActiveScene(int index);
        static void Resize(unsigned int width, unsigned int height) {
            s_Instance->m_Width = width;
            s_Instance->m_Height = height;
        };
    private:
        World(GLuint resX=1920, GLuint resY=1080);
        ~World();

        static World* s_Instance;

        unsigned int m_Width = 1920, m_Height = 1080;

        std::vector<Scene*> m_Scenes;
        Scene* m_ActiveScene;
        FrameBuffer* m_FrameBuffer;
    };

}

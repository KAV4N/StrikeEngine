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


        void AddScene();
        void SetActiveScene(int index);

    private:
        World(GLuint resX=1280, GLuint resY=720);
        ~World();

        static World* s_Instance;

        std::vector<Scene*> m_Scenes;
        Scene* m_ActiveScene;
        FrameBuffer* m_FrameBuffer;
    };

}

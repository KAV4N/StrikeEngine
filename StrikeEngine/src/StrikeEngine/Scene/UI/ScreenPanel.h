#pragma once
#include <vector>

#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Core/Layer.h"
#include "StrikeEngine/Graphics/Core/FrameBuffer.h"
#include "StrikeEngine/Scene/Camera.h"


namespace StrikeEngine {

	class ScreenPanel : public Layer
	{
	public:
		friend class Scene;

		ScreenPanel();
		~ScreenPanel();

		void OnEvent(Event& e) override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;

		void AddScreen(glm::vec2 pos) { m_ScreenViewStack.PushLayer(new ScreenPanel()); }


		Camera* GetCamera() const { return m_Camera; }
		FrameBuffer* GetFrameBuffer() const { return m_FrameBuffer; }

	private:
		void CompositeView();
		
	private:


		LayerStack<Layer*> m_WidgetStack;
		LayerStack<ScreenPanel*> m_ScreenViewStack;

		FrameBuffer* m_FrameBuffer;
		Camera* m_Camera;

		glm::vec2 m_Position;
		glm::vec2 m_Size;
	};
}


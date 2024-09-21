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

		ScreenPanel(glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(1.f));
		~ScreenPanel();

		void OnEvent(Event& e) override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;

		ScreenPanel* AddScreen(glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(1.f));


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


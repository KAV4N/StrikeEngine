
#pragma once

#include <vector>

#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Core/Layer.h"
#include "StrikeEngine/Graphics/Core/FrameBuffer.h"
#include "StrikeEngine/Scene/Camera.h"
#include <StrikeEngine/Scene/Entity.h>



namespace StrikeEngine {

	class RenderLayer : public Layer
	{
	public:
		friend class Scene;

		RenderLayer();
		~RenderLayer();

		void OnEvent(Event& e) override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		std::vector<std::shared_ptr<Camera>> m_Cameras;
		std::vector<Entity> m_LayerObjects;
	};
}


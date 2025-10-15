#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

	class STRIKE_API Input
	{
	public:
		inline static bool IsKeyPressed(int keyCode) { return s_Instance->IsKeyPressedImpl(keyCode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
		inline static std::pair<float, float> GetMouseXY() { return s_Instance->GetMouseXYImpl(); }
		inline static void SetCursorMode(CursorMode mode) { s_Instance->SetCursorModeImpl(mode); };


		//inline static Input* Get() { return s_Instance; }

	protected:
		virtual bool IsKeyPressedImpl(int keyCode) = 0;

		virtual bool IsMouseButtonPressedImpl(int keyCode) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

		virtual std::pair<float, float> GetMouseXYImpl() = 0;
		virtual void SetCursorModeImpl(CursorMode mode) = 0;
	private:
		static Input* s_Instance;
	};
}
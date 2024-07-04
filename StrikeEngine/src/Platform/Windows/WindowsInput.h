#pragma once
#include "StrikeEngine/Core/Input.h"

namespace StrikeEngine {
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int keyCode) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual std::pair<float, float> GetMouseXYImpl() override;
		
	};
}
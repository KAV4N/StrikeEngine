#pragma once

#include "Event.h"

namespace StrikeEngine 
{

	class STRIKE_API WindowResizeEvent : public Event 
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		virtual Event* Clone() const override { return new WindowResizeEvent(m_Width, m_Height); }

	private:
		unsigned int m_Width, m_Height;
	};

	class STRIKE_API WindowCloseEvent : public Event 
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		virtual Event* Clone() const override { return new WindowCloseEvent(); }
	};

	class STRIKE_API AppTickEvent : public Event 
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		virtual Event* Clone() const override { return new AppTickEvent(); }
	};

	class STRIKE_API AppUpdateEvent : public Event 
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		virtual Event* Clone() const override { return new AppUpdateEvent(); }
	};

	class STRIKE_API AppRenderEvent : public Event 
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		virtual Event* Clone() const override { return new AppRenderEvent(); }
	};
}
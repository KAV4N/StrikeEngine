#pragma once

namespace StrikeEngine {

	class FrameBuffer
	{
	public:
		FrameBuffer(unsigned int width, unsigned int height);
		virtual ~FrameBuffer();

		void Invalidate();

		virtual void Bind();
		virtual void Unbind();

		void Resize(unsigned int width, unsigned int height);

		inline unsigned int GetColorAttachment() const { return m_ColorAttachment; }

	private:
		unsigned int m_RendererID = -1;
		unsigned int m_ColorAttachment = -1, m_DepthAttachment = -1;

		unsigned int m_Width, m_Height;
	};

}
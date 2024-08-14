#pragma once
#include "Core/renderer.h"

class FrameBuffer
{
	public:
		FrameBuffer(int width, int height);
		~FrameBuffer();

		void Gen();
		void Bind() const;
		void UnBind() const;

		void Resize(glm::vec2 size);

		unsigned int GetTexture() const;
	private:
		unsigned int bufferID, colourAttachment, depthAttachment;
		int m_width, m_height;
};
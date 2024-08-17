#include "Rendering/framebuffer.h"

FrameBuffer::FrameBuffer(int width, int height)
	: m_width(width), m_height(height)
{

}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteFramebuffers(1, &bufferID));
	GLCall(glDeleteTextures(1, &colourAttachment));
	GLCall(glDeleteRenderbuffers(1, &depthAttachment));
}

void FrameBuffer::Gen()
{
	if (bufferID != NULL)
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

		GLCall(glDeleteFramebuffers(1, &bufferID));
		GLCall(glDeleteTextures(1, &colourAttachment));
		GLCall(glDeleteRenderbuffers(1, &depthAttachment));
	}

	GLCall(glGenFramebuffers(1, &bufferID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, bufferID));

	// Colour attachment.
	GLCall(glGenTextures(1, &colourAttachment));
	GLCall(glBindTexture(GL_TEXTURE_2D, colourAttachment));

	if (m_width < 0) m_width = 0;
	if (m_height < 0) m_height = 0;

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourAttachment, 0)); // Attach texture to framebuffer.
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture.

	// Depth and stencils attachment.
	GLCall(glGenRenderbuffers(1, &depthAttachment));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment));

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height)); // Create depth and stencil.
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment)); // Attach renderbuffer to framebuffer.

	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0)); // Unbind the renderbuffer.

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0)); // Unbind the framebuffer.
}

void FrameBuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, bufferID));
	GLCall(glBindTexture(GL_TEXTURE_2D, colourAttachment));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment));

	GLCall(glViewport(0, 0, m_width, m_height));
}

void FrameBuffer::UnBind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Resize(glm::vec2 size)
{
	m_width = size.x;
	m_height = size.y;

	Gen();
}

glm::vec2 FrameBuffer::GetSize() const
{
	return glm::vec2(m_width, m_height);
}

unsigned int FrameBuffer::GetTexture() const
{
	return colourAttachment;
}
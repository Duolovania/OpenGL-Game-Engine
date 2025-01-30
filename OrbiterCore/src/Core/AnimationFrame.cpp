#include "Components/animationframe.h"

AnimationFrame::AnimationFrame(const std::string& filePath)
	: m_imageFilePath(filePath)
{
	GenerateTextures();
}

void AnimationFrame::GenerateTextures()
{
	m_texture = std::make_unique<Texture>(m_imageFilePath);
	m_texture->Gen();
	m_texture->Bind();

	m_textureIndex = m_texture->GetBufferID();
	m_texture->UnBind();
}

unsigned int AnimationFrame::GetTextureBuffer() const
{
	return m_textureIndex;
}
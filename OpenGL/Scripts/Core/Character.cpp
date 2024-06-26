#include "Core/character.h"

Character::Character(const std::string& imagePath)
{
	m_text = std::make_unique<Texture>("Res/Textures/" + imagePath);
	objectName = imagePath;

	m_text->Bind();
	m_text->Gen();

	texture = m_text->GetBufferID();
	std::cout << objectName + " texture buffer ID: " << texture << std::endl;
	m_text->UnBind();
}
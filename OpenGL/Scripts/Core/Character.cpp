#include "Core/character.h"

Character::Character(const std::string& imagePath)
	:m_imagePath("Res/Textures/" + imagePath)
{
	/*m_text = std::make_unique<Texture>("Res/Textures/" + imagePath);
	objectName = imagePath;

	m_text->Bind();
	m_text->Gen();

	texture = m_text->GetBufferID();
	m_text->UnBind();*/
}
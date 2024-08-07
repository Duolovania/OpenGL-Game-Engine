#include "Core/character.h"

Character::Character(const std::string& imagePath)
	:m_imagePath("Res/Textures/" + imagePath)
{
	objectName = imagePath;
	color = glm::vec4(1.0, 1.0, 1.0, 1.0);
}

// Checks if the image is within the screen space.
bool Character::CheckVisibility(glm::vec2 cameraPosition)
{
	/*if ((transform.position.x >= cameraPosition.x - 50 && transform.position.x <= cameraPosition.x + 250)) return true;
	return false;*/

	return true;
}
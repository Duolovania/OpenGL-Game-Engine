#include "GameObjects/character.h"

Character::Character(const std::string& imagePath)
	//:m_imagePath("Assets/Sprites/" + imagePath)
{
	objectName = imagePath;
	cTexture.m_imagePath = "Assets/Sprites/" + imagePath;

	transform.position = Vector3::zero;
	transform.scale = Vector3(100, 100, 0);
}

// Checks if the image is within the screen space.
bool Character::CheckVisibility(glm::vec2 cameraPosition)
{
	/*if ((transform.position.x >= cameraPosition.x - 50 && transform.position.x <= cameraPosition.x + 250)) return true;
	return false;*/

	return true;
}

void Character::SetColor(glm::vec4 newColor)
{
	for (int i = 0; i < 4; i++)
	{
		color[i] = newColor[i];
	}
}
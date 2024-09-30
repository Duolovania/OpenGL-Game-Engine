#include "GameObjects/character.h"

Character::Character(const std::string& imagePath)
{
	objectName = imagePath;
	cTexture.m_imagePath = "Assets/Sprites/" + imagePath;

	transform.position = Vector3::zero;
	transform.scale = Vector3(100, 100, 0);
}

Character::Character()
{
	objectName = "New Character";
	cTexture.m_imagePath = "../OrbiterCore/Res/Default Sprites/whitesqure.png";

	transform.position = Vector3::zero;
	transform.scale = Vector3(100, 100, 0);
}

// Checks if the image is within the screen space.
bool Character::CheckVisibility(glm::vec2 cameraPosition)
{
	/*if ((transform.position.x >= cameraPosition.x - 50 && transform.position.x <= cameraPosition.x + 250)) return true;
	return false;*/


	////USE THIS
	/*bool checkX = (transform.position.x + transform.scale.x) <= cameraPosition.x + 100 && (transform.position.x + transform.scale.x) >= cameraPosition.x - 100;
	bool checkY = (transform.position.y + transform.scale.y) <= cameraPosition.y + 100 && (transform.position.y + transform.scale.y) >= cameraPosition.y - 100;

	if (checkX && checkY) return true;

	return false;*/






	return true;
}

void Character::SetPath(const std::string& imagePath)
{
	objectName = imagePath;
	cTexture.m_imagePath = "Assets/Sprites/" + imagePath;
}

void Character::SetColor(glm::vec4 newColor)
{
	for (int i = 0; i < 4; i++)
	{
		color[i] = newColor[i];
	}
}
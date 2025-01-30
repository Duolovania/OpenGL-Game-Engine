#include "Components/spriterenderer.h"

SpriteRenderer::SpriteRenderer(const std::string& imagePath)
{
	m_componentName = "Sprite Renderer";
	cTexture.m_imagePath = "Assets/Sprites/" + imagePath;
}

SpriteRenderer::SpriteRenderer()
{
	m_componentName = "Sprite Renderer";
	cTexture.m_imagePath = "../OrbiterCore/Res/Default Sprites/whitesqure.png";
}

// Checks if the image is within the screen space.
bool SpriteRenderer::CheckVisibility(glm::vec2 cameraPosition)
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

void SpriteRenderer::SetPath(const std::string& imagePath)
{
	cTexture.m_imagePath = "Assets/Sprites/" + imagePath;
}

void SpriteRenderer::SetColor(glm::vec4 newColor)
{
	for (int i = 0; i < 4; i++)
	{
		color[i] = newColor[i];
	}
}
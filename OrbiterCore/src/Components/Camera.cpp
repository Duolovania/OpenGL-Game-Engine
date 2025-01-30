#include "Components/camera.h"

void Camera::SetColor(float* targetColor, glm::vec4 newColor)
{
	for (int i = 0; i < 4; i++)
	{
		targetColor[i] = newColor[i];
	}
}
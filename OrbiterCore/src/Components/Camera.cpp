#include "Components/camera.h"

Camera::Camera()
{
	m_componentName = "Camera";
}

void Camera::SetColor(float* targetColor, glm::vec4 newColor)
{
	for (int i = 0; i < 4; i++)
	{
		targetColor[i] = newColor[i];
	}
}
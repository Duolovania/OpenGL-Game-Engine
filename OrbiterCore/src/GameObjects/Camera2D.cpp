#include "GameObjects/camera2d.h"
#include "gtc/matrix_transform.hpp"

Camera2D::Camera2D()
{
	objectName = "Camera2D";
	transform.position = Vector3::zero;
}

void Camera2D::SetColor(float* targetColor, glm::vec4 newColor)
{
	for (int i = 0; i < 4; i++)
	{
		targetColor[i] = newColor[i];
	}
}

glm::mat4 Camera2D::GetView() const
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(-transform.position.x, -transform.position.y, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

#pragma once
#include "GameObjects/gameobject.h"
#include "glm/glm.hpp"

class Camera2D : public GameObject
{
	public:
		Camera2D();
		float outputColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float backgroundColor[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
		void SetColor(float* targetColor, glm::vec4 newColor);

		glm::mat4 GetView() const;
};
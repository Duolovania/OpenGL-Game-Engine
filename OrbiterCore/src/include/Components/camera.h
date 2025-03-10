#pragma once
#include "Components/component.h"
#include "glm/glm.hpp"

class Camera : public Component 
{
	public:
		Camera();

		float outputColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float backgroundColor[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
		void SetColor(float* targetColor, glm::vec4 newColor);
};
#pragma once
#include "Math/transform.h"
#include "Rendering/texture.h"
#include "Core/animationplayer.h"
#include "gtc/matrix_transform.hpp"
#include "Rendering/shader.h"

class Character
{
	public:
		Character(const std::string& imagePath);
		bool CheckVisibility(glm::vec2 cameraPosition);

		unsigned int texture;

		std::string objectName, m_imagePath;
		Transform transform;
		AnimationPlayer animator;

		std::shared_ptr<Shader> m_shader;
		glm::vec4 color;
};
#pragma once
#include "Core/animationplayer.h"
#include "gtc/matrix_transform.hpp"

#include "GameObjects/gameobject.h"
#include "Rendering/texture.h"
#include "Rendering/shader.h"
#include "glad/glad.h"

class Character : public GameObject
{
	public:
		Character(const std::string& imagePath);
		Character();

		bool CheckVisibility(glm::vec2 cameraPosition);
		void SetPath(const std::string& imagePath);

		LiteTexture cTexture;
		AnimationPlayer animator;

		std::shared_ptr<Shader> m_shader;
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		void SetColor(glm::vec4 newColor);
};


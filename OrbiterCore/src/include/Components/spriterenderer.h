#pragma once
#include "Components/component.h"

#include "Components/animationplayer.h"
#include "gtc/matrix_transform.hpp"

#include "Rendering/texture.h"
#include "Rendering/shader.h"
#include "glad/glad.h"

class SpriteRenderer : public Component
{
	public:
		SpriteRenderer(const std::string& imagePath);
		SpriteRenderer();

		bool CheckVisibility(glm::vec2 cameraPosition);
		void SetPath(const std::string& imagePath);

		LiteTexture cTexture;

		std::shared_ptr<Shader> m_shader;
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		void SetColor(glm::vec4 newColor);
};
#pragma once
#include "Components/component.h"

#include "Components/animationplayer.h"
#include "gtc/matrix_transform.hpp"

#include "Rendering/texture.h"
#include "Rendering/shader.h"
#include "glad/glad.h"

// This class stores the sprite renderer data and methods.
class SpriteRenderer : public Component
{
	public:
		/**
		* @brief Specifies the image path upon object creation.
		* 
		* @param imagePath the path of the image. This needs the file extension.
		*/
		SpriteRenderer(const std::string& imagePath);

		// The default constructor.
		SpriteRenderer();

		/**
		* @brief Checks whether the sprite is within the screen bounds.
		* 
		* @param cameraPosition the camera's position. This is a Vector 2.
		* @return whether the sprite is within the screen bounds.
		*/
		bool CheckVisibility(glm::vec2 cameraPosition);

		/**
		* @brief Sets the image path.
		* 
		* @param imagePath the path to the image. This needs the file extension.
		*/
		void SetPath(const std::string& imagePath);

		// The texture object. This stores the bufferID and other texture rendering components.
		LiteTexture cTexture;

		// The shader.
		std::shared_ptr<Shader> m_shader;

		// The colour properties. This is in the normalized RGBA format (ranging from 0.0f to 1.0f).
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		/**
		* @brief Updates the colour properties.
		* 
		* @param newColor The new RGBA values.
		*/
		void SetColor(glm::vec4 newColor);
};
#pragma once
#include "Math/transform.h"
#include "Rendering/texture.h"
#include "Core/animationplayer.h"

class Character
{
	public:
		Character(const std::string& imagePath);
		Transform transform;
		unsigned int texture;

		std::string objectName, m_imagePath;

		AnimationPlayer animator;
};
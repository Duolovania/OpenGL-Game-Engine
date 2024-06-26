#pragma once
#include "Core/animationframe.h"

class Animation
{
	public:
		Animation();
		std::vector<std::shared_ptr<AnimationFrame>> frames;
		std::string animationName;

		unsigned int Start();
	private:
		bool playAnimation = false;
		unsigned int playbackIndex, selectedTexture;
};
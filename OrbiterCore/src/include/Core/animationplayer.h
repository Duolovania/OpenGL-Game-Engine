#pragma once
#include "animation.h"

class AnimationPlayer
{
	public:
		void Play(const std::string& animationName);
	private:
		std::vector<Animation> animations;
};
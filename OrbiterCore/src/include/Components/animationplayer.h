#pragma once
#include "animation.h"
#include "Components/component.h"

class AnimationPlayer : public Component
{
	public:
		void Play(const std::string& animationName);
	private:
		std::vector<Animation> animations;
};
#include "Core/animationplayer.h"

void AnimationPlayer::Play(const std::string& animationName) 
{
	for (auto a : animations)
	{
		if (a.animationName == animationName)
		{
			a.Start();
		}
	}
}
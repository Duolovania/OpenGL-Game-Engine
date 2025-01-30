#include "Components/animation.h"

Animation::Animation()
	: playbackIndex(0), selectedTexture(0)
{
	for (int i = 0; i < frames.size(); i++)
	{
		frames[i] = std::make_unique<AnimationFrame>("Res/Textures/shronk.jpg");
	}
}

unsigned int Animation::Start()
{
	while (playAnimation)
	{
		selectedTexture = frames[playbackIndex]->GetTextureBuffer();

		playbackIndex++;
		if (playbackIndex > frames.size())
			playbackIndex = 0;

		return selectedTexture;
	}
}
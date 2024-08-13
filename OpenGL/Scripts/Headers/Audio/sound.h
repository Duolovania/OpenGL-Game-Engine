#pragma once
#include "Audio/audiosource.h"
#include <string>

struct Sound
{
	std::string soundName, filePath;
	std::shared_ptr<AudioSource> audioSource;

	float pitch = 1.0f, volume = 1.0f;
	bool isLooping = false;
};
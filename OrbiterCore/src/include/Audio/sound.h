#pragma once
#include "Audio/audiosource.h"
#include <string>

struct Sound
{
	std::string soundName, filePath;
	std::shared_ptr<AudioSource> audioSource;

	float pitch = 1.0f, volume = 0.25f;
	bool isLooping = false, playOnStartUp = false, repeatDelay;
	
	glm::vec3 position, velocity;

	Sound& operator=(const Sound& other)
	{
		Sound result;

		result.soundName = other.soundName;
		result.filePath = other.filePath;
		result.audioSource = other.audioSource;
		result.pitch = other.pitch;
		result.volume = other.volume;
		result.isLooping = other.isLooping;
		result.playOnStartUp = other.playOnStartUp;
		result.repeatDelay = other.playOnStartUp;

		return result;
	}

	bool& operator!=(const Sound& other)
	{
		bool result = (
			this->soundName == other.soundName &&
			this->filePath == other.filePath &&
			this->audioSource == other.audioSource &&
			this->pitch == other.pitch &&
			this->volume == other.volume &&
			this->isLooping == other.isLooping &&
			this->playOnStartUp == other.playOnStartUp &&
			this->repeatDelay == other.playOnStartUp) ? false : true;

		return result;
	}

	bool& operator==(const Sound& other)
	{
		bool result = (
			this->soundName == other.soundName &&
			this->filePath == other.filePath &&
			this->audioSource == other.audioSource &&
			this->pitch == other.pitch &&
			this->volume == other.volume &&
			this->isLooping == other.isLooping &&
			this->playOnStartUp == other.playOnStartUp &&
			this->repeatDelay == other.playOnStartUp) ? true : false;

		return result;
	}
};
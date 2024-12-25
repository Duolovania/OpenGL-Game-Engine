#pragma once
#include "Audio/audiosource.h"
#include <string>

// This struct acts as a blueprint for creating/storing sound effects. 
struct Sound
{
	std::string soundName, filePath;
	std::shared_ptr<AudioSource> audioSource;

	float pitch = 1.0f, volume = 0.25f;
	bool isLooping = false, playOnStartUp = false, repeatDelay;
	
	glm::vec3 position, velocity;

	// Overrides the '=' operator to allow for sounds to be set more easily.
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

	// Overrides the '!=' operator to allow for sounds to be compared more easily.
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

	// Overrides the '==' operator to allow for sounds to be compared more easily.
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
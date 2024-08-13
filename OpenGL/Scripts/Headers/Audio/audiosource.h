#pragma once
#include "Audio/audiobuffer.h"
#include "glm/glm.hpp"
#include <iostream>

class AudioSource
{
	public:
		AudioSource(const std::string name);
		~AudioSource();

		void SetProperties(float pitch = 1.0f, float volume = 1.0f, bool looping = false, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0));
		void Play();
		void KillSource();
	private:
		ALuint sourceID;
		std::unique_ptr<AudioBuffer> audioBuffer;
};
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
		void Pause();
		void Stop();
		void KillSource();

		void ChangeFile(const std::string name);

		bool ValidSource() const;

		float m_pitch, m_volume;
		bool m_looping;
		glm::vec3 m_position, m_velocity;
	private:
		ALuint sourceID;
		bool isPaused = false;
		std::string fileName;
		std::unique_ptr<AudioBuffer> audioBuffer;
};
#pragma once
#include "Audio/audiobuffer.h"
#include "glm/glm.hpp"
#include <iostream>

class AudioSource
{
	public:
		/**
		* @brief The Audio Source initializer. This will generate an audio buffer using the selected file.
		*
		* @param name the name of the sound file. This must include directories.
		*/
		AudioSource(const std::string name);
		~AudioSource();

		/**
		* @brief Sets all properties of the audio source.
		*
		* @param pitch the audio pitch.
		* @param volume the audio volume.
		* @param looping the looping status. Setting this as 'true' will loop the audio.
		* @param position the audio position.
		* @param velocity the audio velocity.
		*/
		void SetProperties(float pitch = 1.0f, float volume = 1.0f, bool looping = false, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0));

		// Plays the sound effect.
		void Play();

		// Pauses the sound effect. This can also be used for resuming the sound effect.
		void Pause();

		// Stops the sound effect.
		void Stop();

		// Kills the audio source. This should be used before the application is closed or when the audio source is no longer needed.
		void KillSource();

		/**
		* @brief Changes the selected file.
		*
		* @param name the name of the sound file. This must include directories.
		*/
		void ChangeFile(const std::string name);

		// Checks if the audio source is valid. This is useful for checking whether a sound effect is playing.
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
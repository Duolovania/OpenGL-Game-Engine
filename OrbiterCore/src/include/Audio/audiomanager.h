#pragma once
#include "Audio/sound.h"
#include "Audio/audiocontext.h"

// This class stores methods for audio management.
class AudioManager
{
	public:
		AudioManager();
		~AudioManager();

		std::vector<Sound> sounds; // A 'list' of sounds.

		/**
		* @brief Plays a sound effect.
		* 
		* @param soundName the name of the sound effect.
		*/
		void Play(std::string soundName);

		/**
		* @brief Pauses a specific sound effect.
		*
		* @param soundName the name of the sound effect.
		*/
		void Pause(std::string soundName);

		/**
		* @brief Stops a specific sound effect.
		*
		* @param soundName the name of the sound effect.
		*/
		void Stop(std::string soundName);

		// Plays all sound effects with the 'Play on Start Up' property enabled.
		void PlayOnStartUp() const;

		// Destroys the audio manager. This should be called before the application is terminated.
		void KillAudioManager();

		// Generates all sound effects. This will re-create the audio buffer and sources to 'refresh' the properties for all sound effects.
		void GenAllSounds();

		/**
		* @brief Generates a single sound effect.
		*
		* @param index the index of the sound effect that will be generated.
		*/
		void GenSound(int index);
	private:
		std::unique_ptr<AudioContext> m_audioContext;
		bool CheckDevice();
};
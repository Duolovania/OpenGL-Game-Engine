#pragma once
#include "Audio/sound.h"
#include "Audio/audiocontext.h"

class AudioManager
{
	public:
		AudioManager();
		~AudioManager();

		std::vector<Sound> sounds;
		void Play(std::string soundName);
		void Pause(std::string soundName);

		void Stop(std::string soundName);

		void PlayOnStartUp() const;

		void KillAudioManager();
		void GenSounds();
	private:
		std::unique_ptr<AudioContext> m_audioContext;
		bool CheckDevice();
};
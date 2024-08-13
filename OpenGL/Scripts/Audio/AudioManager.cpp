#include "Audio/audiomanager.h"

AudioManager::AudioManager()
{
	m_audioContext = std::make_unique<AudioContext>();
}

AudioManager::~AudioManager()
{
	for (auto& sound : sounds)
	{
		sound.audioSource->KillSource();
	}

	m_audioContext->KillContext();
}

void AudioManager::Play(std::string soundName)
{
	for (auto& sound : sounds)
	{
		if (sound.soundName == soundName)
		{
			sound.audioSource->Play();
			return;
		}
	}

	std::cout << "Warning: sound with name '" << soundName << "' not found!" << std::endl;
}

void AudioManager::KillAudioManager()
{
	for (auto& sound : sounds)
	{
		sound.audioSource->KillSource();
	}

	m_audioContext->KillContext();
}

void AudioManager::GenSounds()
{
	for (auto& sound : sounds)
	{
		sound.audioSource = std::make_unique<AudioSource>(sound.filePath);
		sound.audioSource->SetProperties(sound.pitch, sound.volume, sound.isLooping);
	}
}
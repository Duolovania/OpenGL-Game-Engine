#include "Audio/audiomanager.h"

AudioManager::AudioManager()
{
	m_audioContext = std::make_unique<AudioContext>();
}

AudioManager::~AudioManager()
{
	if (!CheckDevice()) return;

	for (auto& sound : sounds)
	{
		sound.audioSource->KillSource();
	}

	m_audioContext->KillContext();
}

void AudioManager::Play(std::string soundName)
{
	if (!CheckDevice()) return;

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

void AudioManager::Pause(std::string soundName)
{
	if (!CheckDevice()) return;

	for (auto& sound : sounds)
	{
		if (sound.soundName == soundName)
		{
			sound.audioSource->Pause();
			return;
		}
	}

	std::cout << "Warning: unable to pause sound with name '" << soundName << "'. Sound was not found." << std::endl;
}

void AudioManager::Stop(std::string soundName)
{
	if (!CheckDevice()) return;

	for (auto& sound : sounds)
	{
		if (sound.soundName == soundName)
		{
			sound.audioSource->Stop();
			return;
		}
	}

	std::cout << "Warning: unable to stop sound with name '" << soundName << "'. Sound was not found." << std::endl;
}

void AudioManager::KillAudioManager()
{
	if (!CheckDevice()) return;

	for (auto& sound : sounds)
	{
		sound.audioSource->KillSource();
	}

	m_audioContext->KillContext();
}

void AudioManager::GenSounds()
{
	if (!CheckDevice()) return;

	for (auto& sound : sounds)
	{
		sound.audioSource = std::make_unique<AudioSource>(sound.filePath);
		sound.audioSource->SetProperties(sound.pitch, sound.volume, sound.isLooping);
	}
}

bool AudioManager::CheckDevice()
{
	if (m_audioContext->GetDevice() == nullptr)
	{
		std::cout << "Warning: cannot play audio -- invalid audio device." << std::endl;
		return false;
	}
}

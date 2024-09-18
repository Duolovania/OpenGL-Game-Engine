#include "Audio/audiomanager.h"
#include "Core/debug.h"

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

	DebugOB.Log("Warning: sound with name '" + soundName + "' not found!");
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

	DebugOB.Log("Warning: unable to pause sound with name '" + soundName + "'. Sound was not found.");
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

	DebugOB.Log( "Warning: unable to stop sound with name '" + soundName + "'. Sound was not found.");
}

void AudioManager::PlayOnStartUp() const
{
	for (auto& sound : sounds)
	{
		if (sound.playOnStartUp) sound.audioSource->Play();
	}
}

void AudioManager::KillAudioManager()
{
	if (!CheckDevice()) return;

	for (auto& sound : sounds)
	{
		if (sound.audioSource != nullptr) sound.audioSource->KillSource();
	}

	m_audioContext->KillContext();
}

void AudioManager::GenAllSounds()
{
	if (!CheckDevice()) return;

	for (int i = 0; i < sounds.size(); i++)
	{
		GenSound(i);
	}
}

void AudioManager::GenSound(int index)
{
	if (!CheckDevice()) return;

	if (sounds[index].audioSource == nullptr) sounds[index].audioSource = std::make_unique<AudioSource>(sounds[index].filePath);
	else
	{
		if (!sounds[index].audioSource->ValidSource()) sounds[index].audioSource = std::make_unique<AudioSource>(sounds[index].filePath);
	}
	sounds[index].audioSource->SetProperties(sounds[index].pitch, sounds[index].volume, sounds[index].isLooping, sounds[index].position, sounds[index].velocity);
}

bool AudioManager::CheckDevice()
{
	if (m_audioContext->GetDevice() == nullptr)
	{
		DebugOB.Log("Warning: cannot play audio -- invalid audio device.");
		return false;
	}
}

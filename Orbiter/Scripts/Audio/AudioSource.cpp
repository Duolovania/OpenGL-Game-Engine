#include "Audio/audiosource.h"

AudioSource::AudioSource(const std::string name)
{
    audioBuffer = std::make_unique<AudioBuffer>(name);

    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, audioBuffer->GetBufferID());
}

AudioSource::~AudioSource()
{
    alSourceStop(sourceID);
    alDeleteSources(1, &sourceID);
    audioBuffer->KillBuffer();
}

void AudioSource::SetProperties(float pitch, float volume, bool looping, glm::vec3 position, glm::vec3 velocity)
{
    alSourcef(sourceID, AL_PITCH, pitch);
    alSourcef(sourceID, AL_GAIN, volume);
    alSourcei(sourceID, AL_LOOPING, looping);
    alSource3f(sourceID, AL_POSITION, position.x, position.y, position.z);
    alSource3f(sourceID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void AudioSource::Play()
{
    if (isPaused)
    {
        std::cout << "Unable to play paused sound. Please unpause with 'Pause()' method." << std::endl;
        return;
    }

    alSourcePlay(sourceID);
}

void AudioSource::Pause()
{
    isPaused = !isPaused;

    if (isPaused) alSourcePause(sourceID);
    if (!isPaused) alSourcePlay(sourceID);
}

void AudioSource::Stop()
{
    alSourceStop(sourceID);
}

void AudioSource::KillSource()
{
    alSourceStop(sourceID);
    alDeleteSources(1, &sourceID);
    audioBuffer->KillBuffer();
}
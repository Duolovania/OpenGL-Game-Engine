#include "Audio/audiosource.h"

AudioSource::AudioSource(const std::string name)
{
    audioBuffer = std::make_unique<AudioBuffer>(name);

    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, audioBuffer->GetBufferID());
}

AudioSource::~AudioSource()
{
    alDeleteSources(1, &sourceID);
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
    alSourcePlay(sourceID);
}

void AudioSource::KillSource()
{
    alDeleteSources(1, &sourceID);
    audioBuffer->KillBuffer();
}
#include "Audio/audiosource.h"

AudioSource::AudioSource(const std::string name)
    : fileName(name)
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

    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    // Set reference distance (distance at which sound begins to attenuate).
    alSourcef(sourceID, AL_REFERENCE_DISTANCE, 100.0f); // 1.0f is an example value.

    alSourcef(sourceID, AL_CONE_INNER_ANGLE, 360); // 1.0f is an example value.
    alSourcef(sourceID, AL_CONE_OUTER_ANGLE, 360); // 1.0f is an example value.

    alSource3f(sourceID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

    m_pitch = pitch;
    m_volume = volume;
    m_looping = looping;
    m_position = position;
    m_velocity = velocity;
}

void AudioSource::Play()
{
    if (isPaused)
    {
        std::cout << "Unable to play paused sound. Please unpause with 'Pause()' method." << std::endl;
        return;
    }

    alSourcef(sourceID, AL_PITCH, m_pitch);
    alSourcef(sourceID, AL_GAIN, m_volume);
    alSourcei(sourceID, AL_LOOPING, m_looping);
    alSource3f(sourceID, AL_POSITION, m_position.x, m_position.y, m_position.z);
    alSource3f(sourceID, AL_VELOCITY, m_velocity.x, m_velocity.y, m_velocity.z);

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

void AudioSource::ChangeFile(const std::string name)
{
    if (name == fileName)
    {
        return;
    }

    alSourceStop(sourceID);
    alSourcei(sourceID, AL_BUFFER, 0);

    fileName = name;
    audioBuffer = std::make_unique<AudioBuffer>(name);
    alSourcei(sourceID, AL_BUFFER, audioBuffer->GetBufferID());
}

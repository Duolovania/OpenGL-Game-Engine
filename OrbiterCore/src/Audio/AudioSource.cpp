#include "Audio/audiosource.h"
#include "Core/debug.h"

AudioSource::AudioSource(const std::string name)
    : fileName(name)
{
    audioBuffer = std::make_unique<AudioBuffer>(name);

    if (alIsBuffer(audioBuffer->GetBufferID()))
    {
        alGenSources(1, &sourceID);
        alSourcei(sourceID, AL_BUFFER, audioBuffer->GetBufferID());
    }
}

AudioSource::~AudioSource()
{
    if (ValidSource())
    {
        alSourceStop(sourceID);
        alDeleteSources(1, &sourceID);
    }
    
    audioBuffer->KillBuffer();
}

void AudioSource::SetProperties(float pitch, float volume, bool looping, glm::vec3 position, glm::vec3 velocity)
{
    m_pitch = pitch;
    m_volume = volume;
    m_looping = looping;
    m_position = position;
    m_velocity = velocity;

    if (!ValidSource())
    {
        return;
    }

    alSourcef(sourceID, AL_PITCH, m_pitch);
    alSourcef(sourceID, AL_GAIN, m_volume);
    alSourcei(sourceID, AL_LOOPING, m_looping);
    alSource3f(sourceID, AL_POSITION, m_position.x, m_position.y, m_position.z);

    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    // Set reference distance (distance at which sound begins to attenuate).
    alSourcef(sourceID, AL_REFERENCE_DISTANCE, 100.0f); // 1.0f is an example value.

    alSourcef(sourceID, AL_CONE_INNER_ANGLE, 360); // 1.0f is an example value.
    alSourcef(sourceID, AL_CONE_OUTER_ANGLE, 360); // 1.0f is an example value.

    alSource3f(sourceID, AL_VELOCITY, m_velocity.x, m_velocity.y, m_velocity.z);
}

void AudioSource::Play()
{
    if (isPaused)
    {
        DebugOB.Log("Unable to play paused sound. Please unpause with 'Pause()' method.");
        return;
    }

    if (!ValidSource())
    {
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
    if (!ValidSource())
    {
        return;
    }

    isPaused = !isPaused;

    if (isPaused) alSourcePause(sourceID);
    if (!isPaused) alSourcePlay(sourceID);
}

void AudioSource::Stop()
{
    if (!ValidSource())
    {
        return;
    }

    alSourceStop(sourceID);
}

void AudioSource::KillSource()
{
    if (ValidSource())
    {
        alSourceStop(sourceID);
        alDeleteSources(1, &sourceID);
    }

    audioBuffer->KillBuffer();
}

void AudioSource::ChangeFile(const std::string name)
{
    if (name == fileName)
    {
        return;
    }

    if (ValidSource()) 
    {
        alSourceStop(sourceID);
        alSourcei(sourceID, AL_BUFFER, 0);
    }

    fileName = name;
    audioBuffer = std::make_unique<AudioBuffer>(name);

    if (!ValidSource())
    {
        alGenSources(1, &sourceID);
        alSourcei(sourceID, AL_BUFFER, audioBuffer->GetBufferID());
    }

    alSourcei(sourceID, AL_BUFFER, audioBuffer->GetBufferID());
}

bool AudioSource::ValidSource() const
{
    if (alIsBuffer(sourceID) == AL_TRUE) return true;
    return false;
}

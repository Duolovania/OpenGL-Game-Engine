#include "Audio/audiobuffer.h"
#include <iostream>
#include <bit>
#include <fstream>
#include "Core/debug.h"

AudioBuffer::AudioBuffer(const std::string name)
{
    ALenum format;
    ALsizei freq;
    std::vector<char> data;

    if (!LoadWAVFile(name, bufferID, format, freq, data))
    {
        DebugOB.Log("Failed to locate file: '" + name + "'. Please check the file path.");
    }
}

AudioBuffer::~AudioBuffer()
{
    if (alIsBuffer(bufferID) == AL_FALSE)
    {
        return;
    }

    alDeleteBuffers(1, &bufferID);
}

ALuint AudioBuffer::GetBufferID() const
{
    return bufferID;
}

void AudioBuffer::KillBuffer()
{
    if (alIsBuffer(bufferID) == AL_FALSE)
    {
        return;
    }

    alDeleteBuffers(1, &bufferID);
}

bool AudioBuffer::LoadWAVFile(const std::string& filename, ALuint& buffer, ALenum& format, ALsizei& freq, std::vector<char>& data)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        DebugOB.Log("Failed to open WAV file: '" + filename + "'.");
        return false;
    }

    char chunkId[4];
    file.read(chunkId, 4); // Should be "RIFF"

    if (strncmp(chunkId, "RIFF", 4) != 0) 
    {
        DebugOB.Log("Invalid WAV file: '" + filename + "'.");
        return false;
    }

    file.seekg(20, std::ios::beg);

    uint16_t audioFormat;
    file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));

    if (audioFormat != 1)
    { // Only PCM format is supported
        DebugOB.Log("Unsupported WAV format: " + audioFormat);
        return false;
    }

    uint16_t numChannels;
    file.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));

    uint32_t sampleRate;
    file.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
    freq = sampleRate;

    file.seekg(34, std::ios::beg);

    uint16_t bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));

    // Determine the format
    if (numChannels == 1)
    {
        if (bitsPerSample == 8)
        {
            format = AL_FORMAT_MONO8;
        }
        else if (bitsPerSample == 16)
        {
            format = AL_FORMAT_MONO16;
        }
    }
    else if (numChannels == 2)
    {
        if (bitsPerSample == 8)
        {
            format = AL_FORMAT_STEREO8;
        }
        else if (bitsPerSample == 16)
        {
            format = AL_FORMAT_STEREO16;
        }
    }

    file.seekg(40, std::ios::beg);
    uint32_t dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

    data.resize(dataSize);
    file.read(data.data(), dataSize);

    file.close();

    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data.data(), dataSize, freq);

    return true;
}
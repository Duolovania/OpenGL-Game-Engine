#include "Audio/audiocontext.h"

AudioContext::AudioContext()
{
	device = alcOpenDevice(nullptr); // Opens the default audio device.

	context = alcCreateContext(device, nullptr);
	alcMakeContextCurrent(context);
}

AudioContext::~AudioContext()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void AudioContext::KillContext()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

ALCdevice* AudioContext::GetDevice()
{
	return device;
}

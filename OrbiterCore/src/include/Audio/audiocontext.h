#pragma once
#include "Audio/audiobuffer.h"

class AudioContext
{
	public:
		AudioContext();
		~AudioContext();

		void KillContext();
		ALCdevice* GetDevice();
	private:
		ALCcontext* context;
		ALCdevice* device;
};
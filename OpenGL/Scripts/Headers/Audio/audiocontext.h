#pragma once
#include "Audio/audiobuffer.h"

class AudioContext
{
	public:
		AudioContext();
		~AudioContext();

		void KillContext();
	private:
		ALCcontext* context;
		ALCdevice* device;
};
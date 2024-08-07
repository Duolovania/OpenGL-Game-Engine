#pragma once
#include "Audio/audiobuffer.h"

class AudioSource
{
	public:
		AudioSource();
		~AudioSource();

		void Gen();
	private:
		unsigned int sourceID;
};
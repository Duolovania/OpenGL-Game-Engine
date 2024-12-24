#pragma once
#include "Audio/audiobuffer.h"

// This class stores methods for generating the audio context.
class AudioContext
{
	public:
		// This constructor creates a new audio context. 
		AudioContext();
		~AudioContext();

		// Destroys the audio context. This should be called before the application is terminated.
		void KillContext();

		/**
		* @brief Returns the audio device.
		* 
		* @return the audio device.
		*/
		ALCdevice* GetDevice();
	private:
		ALCcontext* context;
		ALCdevice* device;
};
#pragma once
#include "Audio/audiobuffer.h"
#include <iostream>
#include <bit>
#include <fstream>
#include <vector>

class AudioSource
{
	public:
		AudioSource();
		~AudioSource();

		void Gen();
		bool LoadWAVFile(const std::string& filename, ALuint& buffer, ALenum& format, ALsizei& freq, std::vector<char>& data);

		void Play();

		ALuint source, buffer;
		ALint state;
		ALCdevice* device;
		ALCcontext* context;
	private:
		unsigned int sourceID;
};
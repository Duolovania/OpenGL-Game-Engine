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
		//char* LoadWav(const std::string& filename, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char>& size);
	private:
		unsigned int sourceID;

		/*bool LoadWavHeader(std::ifstream& file, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char>& size);
		std::int32_t ConvertToInt32(char* buffer, std::size_t length);*/


};
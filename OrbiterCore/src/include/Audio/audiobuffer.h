#pragma once
#include "AL/alc.h"
#include "AL/al.h"
#include <vector>
#include <string>

#define ALCall(x) ALClearError();\
    x;\
    ASSERT(ALLogCall(#x, __FILE__, __LINE__))

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)

void ALClearError();

// Use ASSERT(ALLogCall()) to debug.
bool ALLogCall(const char* function, const char* file, int line);

// This class stores methods for generating the audio buffer and loading .wav files.
class AudioBuffer
{
	public:
		/**
		* @brief The audio buffer constructor. 
		*
		* @param name the file path of the .wav file that will be loaded. This requires the file extension.  
		*/
		AudioBuffer(const std::string name);
		~AudioBuffer();
		
		/**
		* @brief Returns the audio buffer ID.
		* 
		* @return the buffer ID.
		*/
		ALuint GetBufferID() const;

		// Deletes the audio buffer. This should be called right before the application is terminated. 
		void KillBuffer();
	private:
		ALuint bufferID;

		/**
		* @brief Loads a .wav file.
		*
		* @param filename 
		* @return the file loading status.
		*/
		bool LoadWAVFile(const std::string& filename, ALuint& buffer, ALenum& format, ALsizei& freq, std::vector<char>& data);
};
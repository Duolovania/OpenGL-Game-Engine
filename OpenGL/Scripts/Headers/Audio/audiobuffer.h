#pragma once
#include "AL/alc.h"
#include "AL/al.h"

#define ALCall(x) ALClearError();\
    x;\
    ASSERT(ALLogCall(#x, __FILE__, __LINE__))

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)



void ALClearError();

// Use ASSERT(ALLogCall()) to debug.
bool ALLogCall(const char* function, const char* file, int line);

class AudioBuffer
{
	public:
		AudioBuffer();
		~AudioBuffer();
	private:
		unsigned int bufferID;
};
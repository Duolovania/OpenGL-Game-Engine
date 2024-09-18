#include "Core/debug.h"

Debug Debug::debugInstance;

void Debug::ClearLog()
{
	consoleBuffer.str("");
	consoleBuffer.clear();
}

std::stringstream& Debug::GetLogOutput()
{
	return consoleBuffer;
}

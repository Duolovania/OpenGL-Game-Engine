#pragma once
#include <iostream>
#include <string>
#include <sstream>

class Debug
{
	public:
		Debug(const Debug&) = delete;

		static Debug& Get()
		{
			return debugInstance;
		}


		template<typename T>
		void Log(const T& message)
		{
			consoleBuffer << message << std::endl;
			//std::cout << message << std::endl; //// Show in console.
		}

		/*void LogWarning(std::string& message);
		void LogError(std::string& message);*/

		void ClearLog();
		std::stringstream& GetLogOutput();
	private:
		std::stringstream consoleBuffer;

		Debug() {};
		static Debug debugInstance;
};

#define DebugOB Debug::Get()
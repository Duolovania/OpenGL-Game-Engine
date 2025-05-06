#pragma once
#include <iostream>
#include <string>
#include <sstream>

// This class provides useful debugging tools.
class Debug
{
	public:
		// The default constructor.
		Debug(const Debug&) = delete;

		// Gets the singleton.
		static Debug& Get()
		{
			return debugInstance;
		}

		/**
		* @brief Messages entered into here will be visible in the editor debug logger.
		* 
		* @param message the message.
		*/
		template<typename T>
		void Log(const T& message)
		{
			consoleBuffer << message << std::endl;
			//std::cout << message << std::endl; //// Show in console.
		}

		/*void LogWarning(std::string& message);
		void LogError(std::string& message);*/

		// Clears all messages in the debug logger.
		void ClearLog();

		/**
		* @brief Gets all logged messages.
		* 
		* @return all messages.
		*/
		std::stringstream& GetLogOutput();
	private:
		std::stringstream consoleBuffer;

		Debug() {};
		static Debug debugInstance;
};

// This macro allows users to access the debugger by calling 'DebugOB.[method]' instead of 'Debug::Get().[method]'.
#define DebugOB Debug::Get()
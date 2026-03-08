#pragma once
#include <string>

// This class stores the script details.
class Script
{
	public:
		/**
		* @brief The default constructor.
		* 
		*/
		Script();

		void SetPath(std::string filePath);
		std::string GetPath() const;
	private:
		// The path where the script file is stored.
		std::string m_path;
};
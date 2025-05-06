#pragma once
#include "Components/component.h"

// This class stores the script details.
class Script : public Component
{
	public:
		/**
		* @brief The default constructor.
		* 
		* @param name the name of the script component. This does not need the file extension.
		*/
		Script(std::string name);

		// The path where the script file is stored.
		std::string filePath;
};
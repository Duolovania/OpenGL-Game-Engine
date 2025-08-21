#pragma once
#include <iostream>
#include "Core/input.h"

// This struct stores the details for project-specific settings.
struct ProjectSettings
{
	// General project settings data.
	std::string name;
	std::string assetsPath;
	std::string projectPath;

	// First loaded scene details.
	std::string firstSceneName;
	std::string firstScenePath;

	// Input actions.
	Input InputManager;

	// Display details.
	float displayResX;
	float displayResY;
};
#pragma once
#include <iostream>
#include "Core/input.h"

// This struct stores the details for project-specific settings.
struct ProjectSettings
{
	// General project settings data.
	std::string name;
	std::string assetsFolderPath;
	std::string projectFilePath;
	std::string projectFolderPath;

	// First loaded scene details.
	std::string firstSceneName;
	std::string firstScenePath;

	// Path to the last scene that was open in the editor.
	std::string recentSceneName;
	std::string recentScenePath;

	// Input actions.
	Input InputManager;

	// Display details.
	float displayResX;
	float displayResY;
};
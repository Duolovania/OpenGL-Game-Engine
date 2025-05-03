#pragma once
#include <iostream>

struct ProjectSettings
{
	// General project settings data.
	std::string name;
	std::string filePath;

	// First loaded scene details.
	std::string firstSceneName;
	std::string firstScenePath;

	// Display details.
	float displayResX;
	float displayResY;
};
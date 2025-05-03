#pragma once
#include <iostream>

struct EditorSettings
{
	// The name of the configuration.
	std::string name = "default";

	// General toggle.
	bool showTooltips = true;

	// Viewport debugging toggle.
	bool showFPS = false;
	bool showWireframe = false;

	// Window toggle.
	bool showRenderingStats = false;
	bool showProjSettings = false;
};
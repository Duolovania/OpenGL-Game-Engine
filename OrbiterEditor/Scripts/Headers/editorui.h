#pragma once
#include "Headers/orbiter.h"

class EditorUI
{
	public:
		void Init(GLFWwindow* window);
		bool Begin();

		void End();
		bool showFPS = false;
	private:
		void StylesConfig();
		bool showStats, wireframeMode, listenToInput;
		int actionIndex = 0, keyBindIndex = 0;
};
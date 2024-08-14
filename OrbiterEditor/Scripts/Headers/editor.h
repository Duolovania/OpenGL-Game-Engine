#pragma once
#include "Headers/orbiter.h"

class Editor : public RenderingLayer
{
	public:
		void Init(GLFWwindow* window) override;

		void OnUpdate(float deltaTime) override;
		void Close() override;
		void CleanUp() override;
	private:
		void StylesConfig();
		bool showStats = false, wireframeMode = false, listenToInput, showFPS = false;
		int actionIndex = 0, keyBindIndex = 0;
};
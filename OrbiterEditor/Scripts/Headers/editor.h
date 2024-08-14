#pragma once
#include "Headers/orbiter.h"

class Editor : public RenderingLayer
{
	public:
		void Init(GLFWwindow* window) override;

		void OnUpdate(float deltaTime) override;
		void Close() override;
		void CleanUp() override;

		bool showFPS = false;
	private:
		void StylesConfig();
		bool showStats, wireframeMode, listenToInput;
		int actionIndex = 0, keyBindIndex = 0;
};
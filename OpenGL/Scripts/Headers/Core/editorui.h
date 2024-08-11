#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Rendering/framebuffer.h"

class EditorUI
{
	public:
		void Init(GLFWwindow* window);
		bool Begin();

		void End();
	private:
		void StylesConfig();
		bool showStats, wireframeMode, listenToInput;
		int actionIndex = 0, keyBindIndex = 0;
};
#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glew.h>
#include <glfw3.h>
#include <iostream>

class Application
{
	public:
		void Init(int screenWidth, int screenHeight, const char* windowTitle);
		void Run();
		void Loop();
		void Close();

		GLFWwindow* window;
};
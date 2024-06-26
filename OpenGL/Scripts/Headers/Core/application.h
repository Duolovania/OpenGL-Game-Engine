#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "Core/input.h"

class Application
{
	public:
		void Init(int screenWidth, int screenHeight, const char* windowTitle);
		void Run();
		void Loop();
		void Close();

		GLFWwindow* window;
};

class Engine
{
	public:
		Engine(const Engine&) = delete;

		static Engine& Get()
		{
			return instance;
		}

		Input InputManager;
		static void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);


	private:
		Engine() {}
		static Engine instance;
};

#define Core Engine::Get()
#define LOG(x) std::cout << x << std::endl;
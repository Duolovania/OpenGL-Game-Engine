#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include "Core/input.h"
#include "Audio/audiomanager.h"
#include "Core/renderinglayer.h"

class Application
{
	public:
		void Init(int screenWidth, int screenHeight, const char* windowTitle);
		void Run();
		void Loop();
		void Close();

		GLFWwindow* window;
	private:
		int m_screenWidth = 1920, m_screenHeight = 1080;
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
		RenderingLayer* renderingLayer;
		std::unique_ptr<AudioManager> audioManager;

		static void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);
		static void HandleGamePadInput(int jid, int event);

	private:
		Engine() {}
		static Engine instance;
};

#define Core Engine::Get()
#define LOG(x) std::cout << x << std::endl;
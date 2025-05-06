#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include "Core/input.h"
#include "Audio/audiomanager.h"
#include "Core/renderinglayer.h"
#include "Core/projectsettings.h"
#include "Scripts/luacontroller.h"

// This is used to define whether the editor, launcher, or solo game is being launched.
enum ApplicationType
{
	LauncherOB,
	EditorOB,
	GameOB
};

// This is used to determine whether the game is playing, paused, or stopped.
enum ApplicationState
{
	Play,
	Pause,
	Stop
};

// This class stores the methods and properties for the application creation, loop, and termination. 
class Application
{
	public:
		/**
		* @brief Initalizes the window and input callbacks.
		* 
		* @param screenWidth the width of the application window.
		* @param screenHeight the height of the application window.
		* @param windowTitle the title of the application window. This will be located on the titlebar.
		*/
		void Init(int screenWidth, int screenHeight, const char* windowTitle);

		// Runs the initalizer and application loop. This should be called to run the application in the entry point (e.g. 'Source.cpp'). 
		void Run();

		// The main application loop.
		void Loop();

		// Terminates the application window.
		void Close();

		/**
		* @brief Updates the screen resolution. By default, this is 1920 x 1080.
		*
		* @param screenWidth the width of the window.
		* @param screenHeight the height of the window.
		*/
		void SetScreenResolution(int screenWidth, int screenHeight);

		GLFWwindow* window;
		ApplicationType applicationType;
	private:
		// 1920 x 1080 by default.
		int m_screenWidth = 1920, m_screenHeight = 1080;
		bool hasStarted = false;
};

// This class stores singleton methods and objects used throughout the program.
class Engine
{
	public:
		Engine(const Engine&) = delete;

		static Engine& Get()
		{
			return instance;
		}

		// The input manager is used for mapping the user's input to controls that can be used in the project.
		RenderingLayer* renderingLayer;
		Renderer renderer;

		ProjectSettings selectedProject;
		LuaController m_scriptController;

		ApplicationState m_applicationState = ApplicationState::Stop;

		// This method handles the keyboard input.
		static void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);

		// This method handles the mouse scroll wheel input.
		static void HandleScrollInput(GLFWwindow* window, double xoffset, double yoffset);
	private:
		Engine() {}
		static Engine instance;
};

// This macro allows for the engine singleton to called as 'Core.[method]' instead of 'Engine::Get().[method]'. 
#define Core Engine::Get()

// This macro allows for easier access to the project settings. Alternatively, you can use 'Core.selectedProject.[method]'.
#define Project Engine::Get().selectedProject
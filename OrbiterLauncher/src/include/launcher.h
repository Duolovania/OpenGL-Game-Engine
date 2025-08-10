#pragma once
#include "orbiter.h"

class Launcher : public RenderingLayer
{
	public:
		void Init(GLFWwindow* window) override;

		bool OnUpdate(float deltaTime, float time) override;
		void Close() override;
	private:
		void StylesConfig();
		void ShowProjects();

		std::string projPath, rootPath;
		LauncherSettings launcherSettings;
		FileManager fileManager;
};
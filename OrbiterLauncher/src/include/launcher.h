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
		void SearchFolders(const std::filesystem::path& folderPath);

		void OpenEditor();
		void ProjectsTable();
		void OpenProjectCreator();

		std::string projPath, rootPath;
		LauncherSettings launcherSettings;
		FileManager fileManager;

		LaunchInstructions launchInstructions;
};
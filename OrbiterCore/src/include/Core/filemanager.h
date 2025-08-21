#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "Core/scene.h"
#include "Core/editorsettings.h"
#include "Core/projectsettings.h"
#include "Core/launchersettings.h"
#include "Core/launchinstructions.h"
#include "yaml-cpp/yaml.h"

// This class handles file creation and loading features.
class FileManager
{
	public:
		/**
		* @brief Creates a new file, storing scene details using the YAML format.
		*
		* @param sceneData the scene object.
		* @param sceneName the name of the scene. This does not take in folders.
		* @param filePath the path to the scene file. This requires the file extension.
		*/
		void CreateSceneFile(Scene sceneData, std::string sceneName, std::string filePath);

		/**
		* @brief Loads scene data from a file that uses the YAML format.
		*
		* @param fileName the name of the scene. This will be visible at the top of the editor.
		* @param filePath the path to the scene file. This requires the file extension.
		* 
		* @return the loaded scene data.
		*/
		Scene LoadSceneFile(std::string fileName, std::string filePath);

		/**
		* @brief Creates a new file, storing details of the editor settings using the YAML format.
		*
		* @param settings the object containing the config data.
		* @param filePath the path to the file. This requires the file extension.
		*/
		void CreateEditorConfig(EditorSettings settings, std::string filePath);

		/**
		* @brief Loads editor config data from a file that uses the YAML format.
		*
		* @param filePath the path to the file. This requires the file extension.
		* @return the loaded editor settings data.
		*/
		EditorSettings LoadEditorConfig(std::string filePath);

		/**
		* @brief Creates a new file, storing details of the project settings using the YAML format.
		*
		* @param settings the object containing the config data.
		* @param filePath the path to the file. This requires the file extension.
		*/
		void CreateProjectConfig(ProjectSettings settings, std::string filePath);

		/**
		* @brief Loads project config data from a file that uses the YAML format.
		*
		* @param filePath the path to the file. This requires the file extension.
		* @return the loaded project settings data.
		*/
		ProjectSettings LoadProjectConfig(std::string filePath);

		/**
		* @brief Opens the file explorer for selecting files.
		*
		* @param filters the accepted file types.
		* @param prompt the prompt shown to the user in the file explorer.
		* @param rootPath the path to the "Assets" folder. Exclude the "Assets" folder when entering the path.
		* 
		* @return the file path that was opened.
		*/
		std::string OpenFileExplorer(const char* filters[], const char* prompt, std::string rootPath);

		/**
		* @brief Saves the file to a specified path in the file explorer.
		*
		* @param filters the accepted file types.
		* @param prompt the prompt shown to the user in the file explorer.
		* @param rootPath the path to the "Assets" folder. Exclude the "Assets" folder when entering the path.
		* 
		* @return the file path that was saved to.
		*/
		const char* SaveFileExplorer(const char* filters[], const char* prompt, std::string rootPath);

		/**
		* @brief Creates a new file, storing details of the launcher settings using the YAML format.
		*
		* @param settings the object containing the config data.
		* @param filePath the path to the file. This requires the file extension.
		*/
		void CreateLauncherConfig(LauncherSettings settings, std::string filePath);

		/**
		* @brief Loads launcher config data from a file that uses the YAML format.
		*
		* @param filePath the launcher to the file. This requires the file extension.
		* @return the loaded editor settings data.
		*/
		LauncherSettings LoadLauncherConfig(std::string filePath);

		/**
		* @brief Creates a new file, storing details of the editor instructions using the YAML format. 
			This is used by the launcher to communicate with the editor, guiding the editor to load the correct project.
		*
		* @param instructions the object containing the instruction data.
		* @param filePath the path to the file. This requires the file extension.
		*/
		void CreateLaunchInstructions(LaunchInstructions instructions, std::string filePath);

		/**
		* @brief Loads editor instructions data from a file that uses the YAML format. 
			This is used by the editor to open the correct project.
		*
		* @param filePath the path to the file. This requires the file extension.
		* @return the loaded editor instructions.
		*/
		LaunchInstructions LoadLaunchInstructions(std::string filePath);
};
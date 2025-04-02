#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "Core/scene.h"
#include "yaml-cpp/yaml.h"

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
		void CreateYAMLFile(Scene sceneData, std::string sceneName, std::string filePath);

		/**
		* @brief Loads scene data from a file that uses the YAML format.
		*
		* @param fileName the name of the scene. This will be visible at the top of the editor.
		* @param filePath the path to the scene file. This requires the file extension.
		*/
		Scene LoadYAMLFile(std::string fileName, std::string filePath);

		/**
		* @brief Opens the file explorer for selecting files.
		*
		* @param filters the accepted file types.
		* @param prompt the prompt shown to the user in the file explorer.
		* @param rootPath the path to the "Assets" folder. Exclude the "Assets" folder when entering the path.
		*/
		std::string OpenFileExplorer(const char* filters[], const char* prompt, std::string rootPath);

		/**
		* @brief Saves the file to a specified path in the file explorer.
		*
		* @param filters the accepted file types.
		* @param prompt the prompt shown to the user in the file explorer.
		* @param rootPath the path to the "Assets" folder. Exclude the "Assets" folder when entering the path.
		*/
		const char* SaveFileExplorer(const char* filters[], const char* prompt, std::string rootPath);
};
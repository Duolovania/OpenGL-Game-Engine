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
		* @brief Creates a new file, storing scene details.
		* 
		* @param sceneData the scene object.
		* @param sceneName the name of the scene. This does not take in folders.
		* @param filePath the path to the scene file. This requires the file extension.
		*/
		void CreateFile(Scene sceneData, std::string sceneName, std::string filePath);

		/**
		* @brief Loads scene data from file.
		* 
		* @param filePath the path to the scene file. This requires the file extension. 
		*/
		Scene LoadFile(std::string filePath);

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
		* @param filePath the path to the scene file. This requires the file extension.
		*/
		Scene LoadYAMLFile(std::string filePath);
};
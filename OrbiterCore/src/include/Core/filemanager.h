#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "Core/scene.h"

class FileManager
{
	public:
		void CreateFile(Scene sceneData, std::string fileName);
		Scene LoadFile(std::string fileName);
};
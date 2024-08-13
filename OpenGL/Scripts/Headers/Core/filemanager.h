#pragma once
#include <iostream>
#include <string>
#include <fstream>

class FileManager
{
	public:
		void CreateFile(std::string fileName, std::string fileExtension);
		void LoadFile();
};
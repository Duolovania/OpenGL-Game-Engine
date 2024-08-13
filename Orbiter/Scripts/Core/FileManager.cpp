#include "Core/filemanager.h"

void FileManager::CreateFile(std::string fileName, std::string fileExtension)
{
	std::ofstream newFile;
	newFile.open(fileName + fileExtension);

	newFile << "hallo";
	newFile.close();
}

void FileManager::LoadFile(std::string fileName, std::string fileExtension)
{
	std::ifstream oldFile;
	std::string text;

	oldFile.open(fileName + fileExtension);

	while (std::getline(oldFile, text))
	{
		std::cout << text << std::endl;
	}

	oldFile.close();
}

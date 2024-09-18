#include "Core/filemanager.h"

void FileManager::CreateFile(Scene sceneData, std::string fileName)
{
	std::ofstream newFile;
	newFile.open(fileName);

	for (auto& data : sceneData.objectsToRender)
	{
		newFile << data.objectName << std::endl;
		newFile << "Position:, " + std::to_string(data.transform.position.x) + ", " << std::to_string(data.transform.position.y) + ", " << std::to_string(data.transform.position.z) << std::endl;
		newFile << "Rotation:, " + std::to_string(data.transform.rotation.x) + ", " << std::to_string(data.transform.rotation.y) + ", " << std::to_string(data.transform.rotation.z) << std::endl;
		newFile << "Scale:, " + std::to_string(data.transform.scale.x) + ", " << std::to_string(data.transform.scale.y) + ", " << std::to_string(data.transform.scale.z) + "\n" << std::endl;
	}

	newFile.close();
}

Scene FileManager::LoadFile(std::string fileName)
{
	std::ifstream oldFile;
	std::string text;
	Scene newScene;

	std::vector<std::string> record;

	oldFile.open(fileName);

	while (std::getline(oldFile, text, ','))
	{
		//record.push_back(text);
		std::cout << text << std::endl;
	}

	oldFile.close();
	return newScene;
}

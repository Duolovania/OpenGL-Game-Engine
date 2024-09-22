#include "Core/filemanager.h"
#include <sstream>

void FileManager::CreateFile(Scene sceneData, std::string fileName)
{
	std::ofstream newFile;
	newFile.open(fileName);

	for (auto& data : sceneData.objectsToRender)
	{
		newFile << "Name: " << data.objectName << std::endl;
		newFile << "Position: " + std::to_string(data.transform.position.x) + " " << std::to_string(data.transform.position.y) + " " << std::to_string(data.transform.position.z) << std::endl;
		newFile << "Rotation: " + std::to_string(data.transform.rotation.x) + " " << std::to_string(data.transform.rotation.y) + " " << std::to_string(data.transform.rotation.z) << std::endl;
		newFile << "Scale: " + std::to_string(data.transform.scale.x) + " " << std::to_string(data.transform.scale.y) + " " << std::to_string(data.transform.scale.z) + "\n" << std::endl;
	}

	newFile.close();
}

Vector3 ParseVector(const std::string& line)
{
	Vector3 vec3;
	std::stringstream ss(line);
	std::string temp;

	ss >> temp;
	ss >> vec3.x >> vec3.y >> vec3.z;

	ss.str("");
	ss.clear();

	return vec3;
}

Scene FileManager::LoadFile(std::string fileName)
{
	std::ifstream oldFile(fileName);
	std::string line;
	Scene newScene;
	Character newChar;

	bool isPosition = false, newCharacter = false;

	if (oldFile.is_open())
	{
		while (std::getline(oldFile, line))
		{
			Vector3 position, rotation, scale;

			if (line.find("Name:") != std::string::npos)
			{
				newChar.objectName = line.substr(line.find("Name:") + 6);
			}
			else if (line.find("Position:") != std::string::npos)
			{
				newChar.transform.position = ParseVector(line);
			}
			else if (line.find("Rotation:") != std::string::npos)
			{
				newChar.transform.rotation = ParseVector(line);
			}
			else if (line.find("Scale:") != std::string::npos)
			{
				newChar.transform.scale = ParseVector(line);
			}
			else
			{
				newCharacter = true;
			}

			if (newCharacter)
			{
				newScene.objectsToRender.push_back(newChar);
				newChar = Character();
				newCharacter = false;
			}
		}

		oldFile.close();
	}

	return newScene;
}

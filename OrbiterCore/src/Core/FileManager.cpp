#include "Core/filemanager.h"
#include <sstream>

enum FileObjectType
{
	None,
	SceneDetails,
	AudioManagerDetails,
	CharacterDetails
};

void FileManager::CreateFile(Scene sceneData, std::string sceneName, std::string filePath)
{
	std::ofstream newFile;
	std::string fullPath = "Assets\\" + filePath;

	newFile.open(fullPath);

	newFile << "Scene: " << sceneName << std::endl;
	newFile << "ScenePath: " << fullPath + "\n" << std::endl;

	newFile << "AudioManager: " << std::endl;

	for (auto& soundEffect : sceneData.audioManager->sounds)
	{
		newFile << "SoundName: " << soundEffect.soundName << std::endl;
		newFile << "SoundFilePath: " << soundEffect.filePath << std::endl;

		newFile << "SoundPitch: " << soundEffect.pitch << std::endl;
		newFile << "SoundVolume: " << soundEffect.volume << std::endl;

		newFile << "SoundLooping: " << soundEffect.isLooping << std::endl;
		newFile << "SoundOnStartUp: " << soundEffect.playOnStartUp << std::endl;
		newFile << "SoundDelay: " << soundEffect.repeatDelay + "\n" << std::endl;
	}

	for (auto& data : sceneData.objectsToRender)
	{
		newFile << "Name: " << data->objectName << std::endl;

		if (std::shared_ptr<Character> character = dynamic_pointer_cast<Character>(data))
		{
			newFile << "ImagePath: " << character->cTexture.m_imagePath << std::endl;
			newFile << "ImageColor: " + std::to_string(character->color[0]) + " " << std::to_string(character->color[1]) + " " << std::to_string(character->color[2]) + " " << std::to_string(character->color[3]) << std::endl;
		}

		if (std::shared_ptr<Camera2D> camera = dynamic_pointer_cast<Camera2D>(data))
		{
			
		}

		newFile << "Rotation: " + std::to_string(data->transform.rotation.x) + " " << std::to_string(data->transform.rotation.y) + " " << std::to_string(data->transform.rotation.z) << std::endl;
		newFile << "Position: " + std::to_string(data->transform.position.x) + " " << std::to_string(data->transform.position.y) + " " << std::to_string(data->transform.position.z) << std::endl;
		newFile << "Scale: " + std::to_string(data->transform.scale.x) + " " << std::to_string(data->transform.scale.y) + " " << std::to_string(data->transform.scale.z) + "\n" << std::endl;
	}

	newFile.close();
}

Vector3 ParseVector3(const std::string& line)
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

Vector4 ParseVector4(const std::string& line)
{
	Vector4 vec4;
	std::stringstream ss(line);
	std::string temp;

	ss >> temp;
	ss >> vec4.x >> vec4.y >> vec4.z >> vec4.w;

	ss.str("");
	ss.clear();

	return vec4;
}

int ParseInt(const std::string& line)
{
	int integer;
	std::stringstream ss(line);
	std::string temp;

	ss >> temp;
	ss >> integer;

	ss.str("");
	ss.clear();

	return integer;
}

Scene FileManager::LoadFile(std::string filePath)
{
	std::ifstream oldFile("Assets" + filePath);
	std::string line;

	Scene newScene;
	std::shared_ptr<Character> newChar = std::make_unique<Character>();

	FileObjectType fObjectType = FileObjectType::None;

	bool isPosition = false, newObject = false;

	if (oldFile.is_open())
	{
		while (std::getline(oldFile, line))
		{
			if (line.find("Scene:") != std::string::npos)
			{
				fObjectType = FileObjectType::SceneDetails;
				newScene.sceneName = line.substr(line.find("Scene:") + 7);
			}
			if (line.find("ScenePath:") != std::string::npos)
			{
				newScene.scenePath = line.substr(line.find("ScenePath:") + 11);
			}

			if (line.find("Name:") != std::string::npos)
			{
				newObject = false;
				fObjectType = FileObjectType::CharacterDetails;
				newChar->objectName = line.substr(line.find("Name:") + 6);
			}
			else if (line.find("Position:") != std::string::npos)
			{
				newChar->transform.position = ParseVector3(line);
			}
			else if (line.find("Rotation:") != std::string::npos)
			{
				newChar->transform.rotation = ParseVector3(line);
			}
			else if (line.find("Scale:") != std::string::npos)
			{
				newChar->transform.scale = ParseVector3(line);
			}
			else if (line.find("ImagePath:") != std::string::npos)
			{
				newChar->cTexture.m_imagePath = line.substr(line.find("ImagePath:") + 11);
			}
			else if (line.find("ImageColor:") != std::string::npos)
			{
				Vector4 vec4 = ParseVector4(line);
				newChar->SetColor({ vec4.x, vec4.y, vec4.z, vec4.w });
			}
			else
			{
				newObject = true;
			}

			if (newObject && fObjectType == FileObjectType::CharacterDetails)
			{
				std::shared_ptr<GameObject> tempgObj = newChar;

				newScene.objectsToRender.push_back(tempgObj);
				newChar = std::make_unique<Character>();
			}
		}

		oldFile.close();
	}

	return newScene;
}

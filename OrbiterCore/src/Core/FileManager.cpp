#include "Core/filemanager.h"
#include "Components/spriterenderer.h"
#include <sstream>

enum FileObjectType
{
	None,
	SceneDetails,
	AudioManagerDetails,
	CharacterDetails
};

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

std::vector<float> Vector3ToString(Vector3 input)
{
	std::vector<float> temp;
	temp.push_back(input.x);
	temp.push_back(input.y);
	temp.push_back(input.z);
	
	return temp;
}

std::vector<float> Vector4ToString(Vector4 input)
{
	std::vector<float> temp;
	temp.push_back(input.x);
	temp.push_back(input.y);
	temp.push_back(input.z);
	temp.push_back(input.w);

	return temp;
}

void FileManager::CreateYAMLFile(Scene sceneData, std::string sceneName, std::string filePath)
{
	YAML::Node yamlNode;

	yamlNode["Scene Details"]["Name"] = sceneName;
	yamlNode["Scene Details"]["Path"] = filePath;

	yamlNode["Audio Manager"] = YAML::Node(YAML::NodeType::Sequence);

	for (auto& soundEffect : sceneData.audioManager->sounds)
	{
		YAML::Node newSoundEffect;

		newSoundEffect["Name"] = soundEffect.soundName;
		newSoundEffect["Path"] = soundEffect.filePath;
		newSoundEffect["Pitch"] = soundEffect.pitch;
		newSoundEffect["Volume"] = soundEffect.volume;

		newSoundEffect["Position"] = Vector3ToString(Vector3(soundEffect.position.x, soundEffect.position.y, soundEffect.position.z));
		newSoundEffect["Velocity"] = Vector3ToString(Vector3(soundEffect.velocity.x, soundEffect.velocity.y, soundEffect.velocity.z));

		newSoundEffect["Looping"] = soundEffect.isLooping;
		newSoundEffect["On Start Up"] = soundEffect.playOnStartUp;
		newSoundEffect["Delay"] = soundEffect.repeatDelay;

		yamlNode["Audio Manager"].push_back(newSoundEffect);
	}

	yamlNode["GameObjects"] = YAML::Node(YAML::NodeType::Sequence);

	for (auto& data : sceneData.objectsToRender)
	{
		YAML::Node newGObj;
		newGObj["Name"] = data->objectName;

		if (data->HasComponent("Sprite Renderer"))
		{
			std::shared_ptr<SpriteRenderer> spriteRenderer = data->GetComponent<SpriteRenderer>();

			newGObj["Sprite Renderer"]["Path"] = spriteRenderer->cTexture.m_imagePath;
			newGObj["Sprite Renderer"]["Color"] = Vector4ToString(Vector4(spriteRenderer->color[0], spriteRenderer->color[1], spriteRenderer->color[2], spriteRenderer->color[3]));
		}

		if (std::shared_ptr<Camera2D> camera = dynamic_pointer_cast<Camera2D>(data))
		{

		}

		newGObj["Transform"]["Position"] = Vector3ToString(data->transform.position);
		newGObj["Transform"]["Rotation"] = Vector3ToString(data->transform.rotation);
		newGObj["Transform"]["Scale"] = Vector3ToString(data->transform.scale);

		yamlNode["GameObjects"].push_back(newGObj);
	}

	YAML::Emitter out;
	out << YAML::Flow;
	out << yamlNode;

	std::ofstream fout(filePath);
	
	if (!fout) {
		std::cerr << "Error: Could not create file at " << filePath << std::endl;
	}

	fout << yamlNode;
	fout.close();
}

Vector3 NodeToVector3(const YAML::Node& root)
{
	std::vector<float> tempVec3 = root.as<std::vector<float>>();
	return Vector3(tempVec3[0], tempVec3[1], tempVec3[2]);
}

Vector4 NodeToVector4(const YAML::Node& root)
{
	std::vector<float> tempVec4 = root.as<std::vector<float>>();
	return Vector4(tempVec4[0], tempVec4[1], tempVec4[2], tempVec4[3]);
}

std::vector<std::shared_ptr<GameObject>> GetGameObjects(const YAML::Node& root)
{
	std::vector<std::shared_ptr<GameObject>> objectVector;

	for (const auto& node : root["GameObjects"])
	{
		SpriteRenderer spriteRenderer;
		std::shared_ptr<GameObject> tempgObj = std::make_unique<GameObject>();

		Vector4 vec4 = NodeToVector4(node["Sprite Renderer"]["Color"]);
		spriteRenderer.SetColor({ vec4.x, vec4.y, vec4.z, vec4.w });
		spriteRenderer.cTexture.m_imagePath = node["Sprite Renderer"]["Path"].as<std::string>();

		tempgObj->AddComponent(spriteRenderer);

		tempgObj->objectName = node["Name"].as<std::string>();

		tempgObj->transform.position = NodeToVector3(node["Transform"]["Position"]);
		tempgObj->transform.rotation = NodeToVector3(node["Transform"]["Rotation"]);
		tempgObj->transform.scale = NodeToVector3(node["Transform"]["Scale"]);

		objectVector.push_back(tempgObj);
		tempgObj = std::make_unique<GameObject>();
	}

	return objectVector;
}

std::vector<Sound> GetSoundEffects(const YAML::Node& root)
{
	std::vector<Sound> soundVector;

	for (const auto& node : root["Audio Manager"])
	{
		Sound newSound;
		newSound.soundName = node["Name"].as<std::string>();
		newSound.filePath = node["Path"].as<std::string>();
		newSound.pitch = node["Pitch"].as<float>();
		newSound.volume = node["Volume"].as<float>();

		Vector3 tempVec3 = NodeToVector3(node["Position"]);
		newSound.position = glm::vec3(tempVec3.x, tempVec3.y, tempVec3.z);

		tempVec3 = NodeToVector3(node["Velocity"]);
		newSound.velocity = glm::vec3(tempVec3.x, tempVec3.y, tempVec3.z);
		
		newSound.isLooping = node["Looping"].as<bool>();
		newSound.playOnStartUp = node["On Start Up"].as<bool>();
		newSound.repeatDelay = node["Delay"].as<bool>();

		soundVector.push_back(newSound);
	}

	return soundVector;
}

Scene FileManager::LoadYAMLFile(std::string filePath)
{
	YAML::Node yamlNode = YAML::LoadFile("Assets" + filePath);
	Scene newScene;

	// Checks if the file exists.
	if (!yamlNode)
	{
		std::cout << "Failed to load file at:" << "Assets" + filePath << std::endl; // Outputs error message.
		return newScene; // Returns empty scene.
	}

	newScene.sceneName = yamlNode["Scene Details"]["Name"].as<std::string>();
	newScene.scenePath = yamlNode["Scene Details"]["Path"].as<std::string>();
	
	newScene.objectsToRender = GetGameObjects(yamlNode);

	newScene.audioManager = std::make_unique<AudioManager>();
	newScene.audioManager->sounds = GetSoundEffects(yamlNode);

	return newScene;
}
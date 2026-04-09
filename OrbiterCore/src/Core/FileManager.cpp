#include "Core/filemanager.h"
#include "Components/spriterenderer.h"
#include "Components/camera.h"
#include "Scripts/scriptmanager.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

#include "Core/input.h"
#include <sstream>

// Converts Vector 3 to string.
std::vector<float> Vector3ToString(glm::vec3 input)
{
	std::vector<float> temp;
	temp.push_back(input.x);
	temp.push_back(input.y);
	temp.push_back(input.z);
	
	return temp;
}

// Converts Vector 4 to string.
std::vector<float> Vector4ToString(glm::vec4 input)
{
	std::vector<float> temp;
	temp.push_back(input.x);
	temp.push_back(input.y);
	temp.push_back(input.z);
	temp.push_back(input.w);

	return temp;
}

// Converts a YAML node to Vector 3.
glm::vec3 NodeToVector3(const YAML::Node& root)
{
	std::vector<float> tempVec3 = root.as<std::vector<float>>();
	return glm::vec3(tempVec3[0], tempVec3[1], tempVec3[2]);
}

// Converts a YAML node to Vector 4.
glm::vec4 NodeToVector4(const YAML::Node& root)
{
	std::vector<float> tempVec4 = root.as<std::vector<float>>();
	return glm::vec4(tempVec4[0], tempVec4[1], tempVec4[2], tempVec4[3]);
}

// Reads a yaml node to generate the vector of sound effects.
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

		glm::vec3 tempVec3 = NodeToVector3(node["Position"]);
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

// Reads a yaml node to generate the vector of scripts.
std::vector<Script> GetScripts(const YAML::Node& root)
{
	std::vector<Script> scripts;

	for (const auto& node : root["Script Manager"])
	{
		Script newScript;
		newScript.SetPath(node["Path"].as<std::string>());
		scripts.push_back(newScript);
	}

	return scripts;
}

// Reads a yaml node to generate the vector of game objects.
std::vector<GameObject> GetGameObjects(const YAML::Node& root)
{
	std::vector<GameObject> objectVector;

	for (const auto& node : root["GameObjects"])
	{
		GameObject tempgObj;

		if (node["Sprite Renderer"])
		{
			SpriteRenderer spriteRenderer;

			glm::vec4 vec4 = NodeToVector4(node["Sprite Renderer"]["Color"]);
			spriteRenderer.SetColor({ vec4.x, vec4.y, vec4.z, vec4.w });
			spriteRenderer.cTexture.m_imagePath = node["Sprite Renderer"]["Path"].as<std::string>();

			tempgObj.AddComponent(spriteRenderer);
		}

		if (node["Camera"])
		{
			Camera camera;

			glm::vec4 vec4 = NodeToVector4(node["Camera"]["Output Color"]);
			camera.SetColor(camera.outputColor, { vec4.x, vec4.y, vec4.z, vec4.w });

			vec4 = NodeToVector4(node["Camera"]["Background Color"]);
			camera.SetColor(camera.backgroundColor, { vec4.x, vec4.y, vec4.z, vec4.w });

			tempgObj.AddComponent(camera);
		}

		if (node["Audio Manager"])
		{
			AudioManager audioManager;

			tempgObj.AddComponent(audioManager);
			tempgObj.GetComponent<AudioManager>()->sounds = GetSoundEffects(node); // Gets the sound effects after the component is added as "AddComponent" initializes the shared pointer so the sound data would be reset.
			tempgObj.GetComponent<AudioManager>()->GenAllSounds();
		}

		if (node["Script Manager"])
		{
			ScriptManager scriptManager;

			tempgObj.AddComponent(scriptManager);
			tempgObj.GetComponent<ScriptManager>()->SetScripts(GetScripts(node));
		}

		tempgObj.objectName = node["Name"].as<std::string>();

		tempgObj.transform.position = NodeToVector3(node["Transform"]["Position"]);
		tempgObj.transform.rotation = NodeToVector3(node["Transform"]["Rotation"]);
		tempgObj.transform.scale = NodeToVector3(node["Transform"]["Scale"]);

		objectVector.push_back(tempgObj);
	}

	return objectVector;
}

// Reads a yaml node to generate the vector of input actions.
std::vector<Action> GetActions(const YAML::Node& root)
{
	std::vector<Action> newActionList; // Creates a blank vector of actions.

	// Loops through each action node.
	for (const auto& actionNode : root["Project Details"]["Input Actions"])
	{
		Action newAction = Action(actionNode["Name"].as<std::string>()); // Gets the name of the action.

		// Loops through each keybind node.
		for (const auto& keyBindNode : actionNode["Keybinds"])
		{
			newAction.AddKeyBind(keyBindNode["Key Code"].as<int>()); // Adds the keybind to the list of keybinds.
		}

		newActionList.push_back(newAction); // Adds the action to the list of actions.
	}

	return newActionList;
}

// Creates the scene file using the scene data.
void FileManager::CreateSceneFile(Scene sceneData, std::string sceneName, std::string filePath)
{
	YAML::Node yamlNode;

	// Sets the scene file header details.
	yamlNode["Scene Details"]["Name"] = sceneName;
	yamlNode["Scene Details"]["Path"] = filePath;

	// Creates the list for game objects in the file structure.
	yamlNode["GameObjects"] = YAML::Node(YAML::NodeType::Sequence);

	// Loops through each game object.
	for (auto& data : sceneData.objectsToRender)
	{
		YAML::Node newGObj;
		newGObj["Name"] = data.objectName; // Sets the game object name.

		// Checks if the object has a sprite renderer component.
		if (data.HasComponent("Sprite Renderer"))
		{
			SpriteRenderer spriteRenderer = *data.GetComponent<SpriteRenderer>();

			// Copies the component properties.
			newGObj["Sprite Renderer"]["Path"] = spriteRenderer.cTexture.m_imagePath;
			newGObj["Sprite Renderer"]["Color"] = Vector4ToString(glm::vec4(spriteRenderer.color[0], spriteRenderer.color[1], spriteRenderer.color[2], spriteRenderer.color[3]));
		}

		// Checks if the object has a camera component.
		if (data.HasComponent("Camera"))
		{
			Camera camera = *data.GetComponent<Camera>();

			// Copies the component properties.
			newGObj["Camera"]["Output Color"] = Vector4ToString(glm::vec4(camera.outputColor[0], camera.outputColor[1], camera.outputColor[2], camera.outputColor[3]));
			newGObj["Camera"]["Background Color"] = Vector4ToString(glm::vec4(camera.backgroundColor[0], camera.backgroundColor[1], camera.backgroundColor[2], camera.backgroundColor[3]));
		}

		// Checks if the object has an audio manager component.
		if (data.HasComponent("Audio Manager"))
		{
			newGObj["Audio Manager"] = YAML::Node(YAML::NodeType::Sequence); // Creates the 'Audio Manager' umbrella.

			AudioManager audioManager = *data.GetComponent<AudioManager>();

			// Loops through each sound effect in the list of sounds.
			for (auto& soundEffect : audioManager.sounds)
			{
				YAML::Node newSoundEffect;

				// Copies the sound effect properties.
				newSoundEffect["Name"] = soundEffect.soundName;
				newSoundEffect["Path"] = soundEffect.filePath;
				newSoundEffect["Pitch"] = soundEffect.pitch;
				newSoundEffect["Volume"] = soundEffect.volume;

				newSoundEffect["Position"] = Vector3ToString(soundEffect.position);
				newSoundEffect["Velocity"] = Vector3ToString(soundEffect.velocity);

				newSoundEffect["Looping"] = soundEffect.isLooping;
				newSoundEffect["On Start Up"] = soundEffect.playOnStartUp;
				newSoundEffect["Delay"] = soundEffect.repeatDelay;

				newGObj["Audio Manager"].push_back(newSoundEffect); // Adds the sound effect details to the yaml data.
			}
		}

		// Checks if the object has a script manager component.
		if (data.HasComponent("Script Manager"))
		{
			newGObj["Script Manager"] = YAML::Node(YAML::NodeType::Sequence); // Creates the 'Script Manager' umbrella.

			ScriptManager scriptManager = *data.GetComponent<ScriptManager>();

			// Loops through each script in the list of scripts.
			for (auto& script : scriptManager.GetScripts())
			{
				YAML::Node newScript;

				// Copies script properties.
				newScript["Path"] = script.GetPath();

				newGObj["Script Manager"].push_back(newScript); // Adds the script details to the yaml data.
			}
		}

		// Copies the transform data.
		newGObj["Transform"]["Position"] = Vector3ToString(data.transform.position);
		newGObj["Transform"]["Rotation"] = Vector3ToString(data.transform.rotation);
		newGObj["Transform"]["Scale"] = Vector3ToString(data.transform.scale);

		yamlNode["GameObjects"].push_back(newGObj); // Adds the game object to the yaml data.
	}

	YAML::Emitter out;
	out << YAML::Flow;
	out << yamlNode;

	// Create file at the file path.
	std::ofstream fout(filePath);
	
	// Checks if the file was not created.
	if (!fout) {
		std::cerr << "Error: Could not create file at " << filePath << std::endl;
	}

	fout << yamlNode; // Writes data to file.
	fout.close(); // Closes the file.
}

// Loads the YAML file to scene data.
Scene FileManager::LoadSceneFile(std::string fileName, std::string filePath)
{
	YAML::Node yamlNode = YAML::LoadFile(filePath);
	Scene newScene;

	// Checks if the file exists.
	if (!yamlNode)
	{
		std::cout << "Failed to load file at:" << filePath << std::endl; // Outputs error message.
		return newScene; // Returns empty scene.
	}

	newScene.sceneName = fileName;
	newScene.scenePath = filePath;

	newScene.objectsToRender = GetGameObjects(yamlNode);

	return newScene;
}

// Creates an editor config file using editor settings data.
void FileManager::CreateEditorConfig(EditorSettings settings, std::string filePath)
{
	YAML::Node yamlNode;

	// Sets the name of the editor configuration.
	yamlNode["Editor Details"]["Name"] = settings.name;

	// Sets the general values.
	yamlNode["Editor Details"]["General"]["Show Tooltips"] = settings.showTooltips;

	// Sets the viewport debugging values.
	yamlNode["Editor Details"]["Viewport"]["Show FPS"] = settings.showFPS;
	yamlNode["Editor Details"]["Viewport"]["Show Wire Frame"] = settings.showWireframe;

	// Sets the window toggle values.
	yamlNode["Editor Details"]["Window"]["Show Rendering Stats"] = settings.showRenderingStats;
	yamlNode["Editor Details"]["Window"]["Show Project Settings"] = settings.showProjSettings;
	yamlNode["Editor Details"]["Window"]["Show Console Window"] = settings.showConsoleWindow;

	YAML::Emitter out;
	out << YAML::Flow;
	out << yamlNode;

	// Create file at the file path.
	std::ofstream fout(filePath);

	// Checks if the file was not created.
	if (!fout) {
		std::cerr << "Error: Could not create file at " << filePath << std::endl;
	}

	fout << yamlNode; // Writes editor data to file.
	fout.close(); // Closes the file.
}

// Loads the YAML file to editor config data.
EditorSettings FileManager::LoadEditorConfig(std::string filePath)
{
	EditorSettings newEditorConfig;

	try 
	{
		YAML::Node yamlNode = YAML::LoadFile(filePath);

		// Checks if the file exists.
		if (!yamlNode)
		{
			std::cout << "Failed to load file at:" << filePath << std::endl; // Outputs error message.
			return newEditorConfig; // Returns empty scene.
		}

		// Sets the config name.
		newEditorConfig.name = yamlNode["Editor Details"]["Name"].as<std::string>();

		// Sets the general values.
		newEditorConfig.showTooltips = yamlNode["Editor Details"]["General"]["Show Tooltips"].as<bool>();

		// Sets the viewport debug toggle values.
		newEditorConfig.showFPS = yamlNode["Editor Details"]["Viewport"]["Show FPS"].as<bool>();
		newEditorConfig.showWireframe = yamlNode["Editor Details"]["Viewport"]["Show Wire Frame"].as<bool>();

		// Sets the window toggle values.
		newEditorConfig.showRenderingStats = yamlNode["Editor Details"]["Window"]["Show Rendering Stats"].as<bool>();
		newEditorConfig.showWireframe = yamlNode["Editor Details"]["Window"]["Show Project Settings"].as<bool>();
		newEditorConfig.showConsoleWindow = yamlNode["Editor Details"]["Window"]["Show Console Window"].as<bool>();
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Could not find editor config file. Loading default configuration." << std::endl;
	}

	return newEditorConfig;
}

// Creates project settings file using project settings data.
void FileManager::CreateProjectConfig(ProjectSettings settings, std::string filePath)
{
	YAML::Node yamlNode;

	// Sets the general data of the project.
	yamlNode["Project Details"]["Name"] = settings.name;
	yamlNode["Project Details"]["Path"] = settings.assetsFolderPath;

	// Sets the first scene data.
	yamlNode["Project Details"]["First Scene"]["Name"] = settings.firstSceneName;
	yamlNode["Project Details"]["First Scene"]["Path"] = settings.firstScenePath;

	// Sets the display properties.
	yamlNode["Project Details"]["Display"]["Resolution"]["X"] = settings.displayResX;
	yamlNode["Project Details"]["Display"]["Resolution"]["Y"] = settings.displayResY;

	// Creates the list for input actions in the file structure.
	yamlNode["Project Details"]["Input Actions"] = YAML::Node(YAML::NodeType::Sequence);

	// Loops through each input action.
	for (auto& data : settings.InputManager.actionList)
	{
		YAML::Node newAction;

		newAction["Name"] = data.GetActionName(); // Sets the action name.
		newAction["Keybinds"] = YAML::Node(YAML::NodeType::Sequence); // Creates the 'Keybinds' umbrella.

		// Loops through each keybind in the list of keybinds.
		for (auto& keyBind : data.GetKeyBinds())
		{
			YAML::Node newKeyBind;

			newKeyBind["Key Code"] = keyBind; // Copies the key code value.
			newAction["Keybinds"].push_back(newKeyBind); // Adds the key code to the list of keybinds.
		}

		yamlNode["Project Details"]["Input Actions"].push_back(newAction); // Adds the action to the yaml data.
	}

	YAML::Emitter out;
	out << YAML::Flow;
	out << yamlNode;

	// Create file at the file path.
	std::ofstream fout(filePath);

	// Checks if the file was not created.
	if (!fout) {
		std::cerr << "Error: Could not create file at " << filePath << std::endl;
	}

	fout << yamlNode; // Writes editor data to file.
	fout.close(); // Closes the file.
}

// Loads the YAML file to project settings data.
ProjectSettings FileManager::LoadProjectConfig(std::string filePath)
{
	ProjectSettings newProjectConfig;

	try
	{
		YAML::Node yamlNode = YAML::LoadFile(filePath);

		// Checks if the file exists.
		if (!yamlNode)
		{
			std::cout << "Failed to load file at:" << filePath << std::endl; // Outputs error message.
			return newProjectConfig; // Returns nothing.
		}

		// Sets the general values.
		newProjectConfig.name = yamlNode["Project Details"]["Name"].as<std::string>();
		newProjectConfig.assetsFolderPath = yamlNode["Project Details"]["Path"].as<std::string>();

		// Sets the general values.
		newProjectConfig.firstSceneName = yamlNode["Project Details"]["First Scene"]["Name"].as<std::string>();
		newProjectConfig.firstScenePath = yamlNode["Project Details"]["First Scene"]["Path"].as<std::string>();

		// Sets the display values.
		newProjectConfig.displayResX = yamlNode["Project Details"]["Display"]["Resolution"]["X"].as<float>();
		newProjectConfig.displayResY = yamlNode["Project Details"]["Display"]["Resolution"]["Y"].as<float>();

		// Checks if any actions were found before overwriting the default action list.
		std::vector<Action> newActionList = GetActions(yamlNode);
		if (newActionList.size() > 0) newProjectConfig.InputManager.actionList = newActionList;
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Could not find project file." << std::endl;
	}

	return newProjectConfig;
}

// Opens the system file explorer.
std::string FileManager::OpenFileExplorer(const char* filters[], const char* prompt, std::string rootPath)
{
	const char* filePath = tinyfd_openFileDialog(
		prompt,              // Title of the dialog
		(rootPath + "/Assets").c_str(),                         // Default path ("" means current directory)
		1,                          // Number of file filters
		filters,                // File filters (e.g., ["*.txt"])
		NULL,                       // Filter description
		0                           // Allow multiple selections (0 for no)
	);

	if (!filePath) return "";

	return std::string(filePath);
}

// Saves a file through the file explorer.
const char* FileManager::SaveFileExplorer(const char* filters[], const char* prompt, std::string rootPath)
{
	const char* savePath = tinyfd_saveFileDialog(
		prompt,              // Title of the dialog.
		(rootPath + "/Assets/").c_str(), // Default name.
		1,                         // Number of file filters.
		filters,               // File filters (e.g., ["*.txt"]).
		NULL                       // Filter description.
	);

	return savePath;
}

void FileManager::CreateLauncherConfig(LauncherSettings settings, std::string filePath)
{
	YAML::Node yamlNode;

	// Sets the general data of the project.
	yamlNode["Directories"] = YAML::Node(YAML::NodeType::Sequence); // Creates the 'Directories' umbrella.

	// Loops through each path in the list of directories.
	for (auto& path : settings.directories)
	{
		YAML::Node newDirectory;

		// Copies the sound effect properties.
		newDirectory["Path"] = path;

		yamlNode["Directories"].push_back(newDirectory); // Adds the path details to the yaml data.
	}

	// Sets the general data of the project.
	yamlNode["Scan Directories"] = YAML::Node(YAML::NodeType::Sequence); // Creates the 'Directories' umbrella.

	// Loops through each path in the list of directories.
	for (auto& path : settings.scanDirectories)
	{
		YAML::Node newDirectory;

		// Copies the sound effect properties.
		newDirectory["Path"] = path;

		yamlNode["Scan Directories"].push_back(newDirectory); // Adds the path details to the yaml data.
	}

	YAML::Emitter out;
	out << YAML::Flow;
	out << yamlNode;

	// Create file at the file path.
	std::ofstream fout(filePath);

	// Checks if the file was not created.
	if (!fout) {
		std::cerr << "Error: Could not create file at " << filePath << std::endl;
	}

	fout << yamlNode; // Writes launcher data to file.
	fout.close(); // Closes the file.
}

// Reads a yaml node to generate the vector of directories.
std::vector<std::string> GetDirectories(const YAML::Node& root, const char* nodeName = "Directories")
{
	std::vector<std::string> directories; // Creates a blank vector of actions.

	// Loops through each action node.
	for (const auto& pathNode : root[nodeName])
	{
		std::string newPath = pathNode["Path"].as<std::string>(); // Gets the name of the action.
		directories.push_back(newPath); // Adds the action to the list of actions.
	}

	return directories;
}

LauncherSettings FileManager::LoadLauncherConfig(std::string filePath)
{
	LauncherSettings newLauncherConfig;

	try
	{
		YAML::Node yamlNode = YAML::LoadFile(filePath);

		// Checks if the file exists.
		if (!yamlNode)
		{
			std::cout << "Failed to load file at:" << filePath << std::endl; // Outputs error message.
			return newLauncherConfig; // Returns empty scene.
		}

		// Checks if any actions were found before overwriting the default action list.
		std::vector<std::string> newDirectories = GetDirectories(yamlNode);
		if (newDirectories.size() > 0) newLauncherConfig.directories = newDirectories;

		// Checks if any actions were found before overwriting the default action list.
		std::vector<std::string> newScanDirectories = GetDirectories(yamlNode, "Scan Directories");
		if (newScanDirectories.size() > 0) newLauncherConfig.scanDirectories = newScanDirectories;
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Could not find launcher file. Loading default launcher settings." << std::endl;
	}

	return newLauncherConfig;
}

void FileManager::CreateLaunchInstructions(LaunchInstructions instructions, std::string filePath)
{
	YAML::Node yamlNode;

	// Sets the name of the editor configuration.
	yamlNode["Selected Project"]["Path"] = instructions.selectedProjPath;

	YAML::Emitter out;
	out << YAML::Flow;
	out << yamlNode;

	// Create file at the file path.
	std::ofstream fout(filePath);

	// Checks if the file was not created.
	if (!fout) {
		std::cerr << "Error: Could not create file at " << filePath << std::endl;
	}

	fout << yamlNode; // Writes data to file.
	fout.close(); // Closes the file.
}

LaunchInstructions FileManager::LoadLaunchInstructions(std::string filePath)
{
	LaunchInstructions newLaunchInstructions;

	try
	{
		YAML::Node yamlNode = YAML::LoadFile(filePath);

		// Checks if the file exists.
		if (!yamlNode)
		{
			std::cout << "Failed to load file at:" << filePath << std::endl; // Outputs error message.
			return newLaunchInstructions; // Returns empty scene.
		}

		// Sets the project path.
		newLaunchInstructions.selectedProjPath = yamlNode["Selected Project"]["Path"].as<std::string>();
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Could not find editor config file. Loading default configuration." << std::endl;
	}

	return newLaunchInstructions;
}

#include "game.h"
#include <filesystem>

void Game::Init(GLFWwindow* window)
{
	Core.renderer.Init();
	//Core.selectedProject.filePath = std::filesystem::current_path().parent_path().string() + "\\Projects\\" + Core.selectedProject.name + "\\Assets"; // Sets the folder where the assets are located.

	FileManager fileManager;
	currentScene = fileManager.LoadSceneFile(Core.selectedProject.firstSceneName, Core.selectedProject.firstScenePath); // Loads the first scene.
	Core.renderer.objectsToRender = currentScene.objectsToRender;
}

bool Game::OnUpdate(float deltaTime, float time)
{
	return false;
}

void Game::Close()
{

}

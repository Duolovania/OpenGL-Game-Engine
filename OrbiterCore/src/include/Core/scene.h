#pragma once
#include "GameObjects/character.h"
#include "GameObjects/camera2d.h"
#include <vector>
#include <string>

class Scene
{
	public:
		std::string sceneName, scenePath;
		std::vector<std::shared_ptr<GameObject>> objectsToRender;
};
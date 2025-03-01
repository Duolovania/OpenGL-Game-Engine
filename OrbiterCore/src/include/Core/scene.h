#pragma once
#include "GameObjects/gameobject.h"
#include "Audio/audiomanager.h"

#include <vector>
#include <string>

class Scene
{
	public:
		std::string sceneName, scenePath;
		std::vector<std::shared_ptr<GameObject>> objectsToRender;

		std::shared_ptr<AudioManager> audioManager;
};
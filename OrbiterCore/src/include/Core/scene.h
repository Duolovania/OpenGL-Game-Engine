#pragma once
#include "GameObjects/gameobject.h"
#include "Audio/audiomanager.h"

#include <vector>
#include <string>

class Scene
{
	public:
		std::string sceneName, scenePath;
		std::vector<GameObject> objectsToRender;
		int startCameraIndex = -1;
};
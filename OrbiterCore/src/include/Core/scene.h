#pragma once
#include "GameObjects/character.h"
#include <vector>
#include <string>

class Scene
{
	public:
		std::string sceneName;
		std::vector<Character> objectsToRender;
};
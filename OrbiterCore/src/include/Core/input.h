#pragma once
#include "Core/action.h"
#include "glm/glm.hpp"
#include <map>

class Input
{
	public:
		std::vector<Action> actionList;

		void DeleteAction(int index);
		void AddAction(Action newAction);
		int GetActionStrength(const std::string actionName);

		glm::vec2 BasicMovement();

		Input();
};
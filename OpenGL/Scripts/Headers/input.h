#pragma once
#include "Headers/action.h"
#include <map>

class Input
{
	public:
		int GetActionStrength(const std::string actionName);
		std::map<int, Action> actionMap;

		Input();
};
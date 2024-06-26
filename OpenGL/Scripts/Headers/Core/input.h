#pragma once
#include "Core/action.h"
#include <map>

class Input
{
	public:
		std::vector<Action> actionList;

		void DeleteAction(int index);
		void AddAction(Action newAction);
		int GetActionStrength(const std::string actionName);

		Input();
};
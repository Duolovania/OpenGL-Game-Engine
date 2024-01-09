#include "Headers/input.h"

Input::Input()
{
	actionMap.insert({ {GLFW_KEY_D, Action("right")},  {GLFW_KEY_A, Action("left")}, {GLFW_KEY_W, Action("up")},  {GLFW_KEY_S, Action("down")} });
};

int Input::GetActionStrength(const std::string actionName)
{
	for (auto a : actionMap)
	{
		if (a.second.GetActionName() == actionName) return a.second.GetStrength();
	}

	std::cout << "Input action with name: " << actionName << " was not found." << std::endl;
	return 0;
};
#include "Headers/input.h"

Input::Input()
{
	actionMap.insert({ {GLFW_KEY_D, Action("right")},  {GLFW_KEY_A, Action("left")}, {GLFW_KEY_W, Action("up")},  {GLFW_KEY_S, Action("down")}, 
		{GLFW_KEY_RIGHT, Action("arrowRight")},  {GLFW_KEY_LEFT, Action("arrowLeft")}, {GLFW_KEY_UP, Action("arrowUp")},  {GLFW_KEY_DOWN, Action("arrowDown")}, {GLFW_KEY_LEFT_SHIFT, Action("sprint")} });
};

// Verifies that the action exists and returns the value.
int Input::GetActionStrength(const std::string actionName)
{
	for (auto a : actionMap)
	{
		if (a.second.GetActionName() == actionName) return a.second.GetStrength();
	}

	std::cout << "Input action with name: " << actionName << " was not found." << std::endl;
	return 0;
};
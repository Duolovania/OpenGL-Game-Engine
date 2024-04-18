#include "Headers/input.h"

Input::Input()
{
	actionList.push_back(Action("right", GLFW_KEY_D));
	actionList.push_back(Action("left", GLFW_KEY_A));
	actionList.push_back(Action("down", GLFW_KEY_S));
	actionList.push_back(Action("up", GLFW_KEY_W));

	actionList.push_back(Action("arrowRight", GLFW_KEY_RIGHT));
	actionList.push_back(Action("arrowLeft", GLFW_KEY_LEFT));
	actionList.push_back(Action("arrowDown", GLFW_KEY_DOWN));
	actionList.push_back(Action("arrowUp", GLFW_KEY_UP));

	actionList.push_back(Action("sprint", GLFW_KEY_LEFT_SHIFT));

	actionList[0].AddKeyBind(GLFW_KEY_RIGHT);
};

// Verifies that the action exists and returns the value.
int Input::GetActionStrength(const std::string actionName)
{
	for (int i = 0; i < actionList.size(); i++)
	{
		if (actionList[i].GetActionName() == actionName) return actionList[i].GetStrength();
	}

	std::cout << "Input action with name: " << actionName << " was not found." << std::endl;
	return 0;
};
#include "Core/input.h"

Input::Input()
{
	AddAction(Action("right", GLFW_KEY_D));
	AddAction(Action("left", GLFW_KEY_A));
	AddAction(Action("down", GLFW_KEY_S));
	AddAction(Action("up", GLFW_KEY_W));

	AddAction(Action("arrowRight", GLFW_KEY_RIGHT));
	AddAction(Action("arrowLeft", GLFW_KEY_LEFT));
	AddAction(Action("arrowDown", GLFW_KEY_DOWN));
	AddAction(Action("arrowUp", GLFW_KEY_UP));

	AddAction(Action("sprint", GLFW_KEY_LEFT_SHIFT));

	//actionList[0].AddKeyBind(GLFW_KEY_RIGHT);
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
}

glm::vec2 Input::BasicMovement()
{
	return glm::vec2(GetActionStrength("right") - GetActionStrength("left"), GetActionStrength("up") - GetActionStrength("down"));
};	

// Removes an action from the list.
void Input::DeleteAction(int index)
{
	actionList.erase(actionList.begin() + index);
};

// Adds a new action if there one does not exist with the same name.
void Input::AddAction(Action newAction)
{
	if (std::find(actionList.begin(), actionList.end(), newAction) != actionList.end()) return;

	actionList.push_back(newAction);
};
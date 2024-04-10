#include "Headers/action.h"

Action::Action(const std::string actionName)
	: actionName(actionName), strength(0)
{

};

// Returns the result of the key pressed event given from 'SetStrength()'.
int Action::GetStrength() const
{
	return strength;
};

// Checks if the set key has been pressed.
void Action::SetStrength(int glfwAction)
{
	strength = (glfwAction == GLFW_RELEASE) ? 0 : 1;
};

// Returns the name of the action.
const std::string Action::GetActionName() const
{
	return actionName;
};
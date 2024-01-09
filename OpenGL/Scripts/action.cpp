#include "Headers/action.h"

Action::Action(const std::string actionName)
	: actionName(actionName), strength(0)
{

};

int Action::GetStrength() const
{
	return strength;
};

void Action::SetStrength(int glfwAction)
{
	strength = (glfwAction == GLFW_RELEASE) ? 0 : 1;
};

const std::string Action::GetActionName() const
{
	return actionName;
};
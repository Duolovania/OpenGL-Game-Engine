#include "Headers/action.h"

Action::Action(const std::string actionName, int keyBind)
	: actionName(actionName), strength(0)
{
	keybinds.push_back(keyBind);
};

// Returns the result of the key pressed event given from 'SetStrength()'.
int Action::GetStrength() const
{
	return strength;
};

// Returns the selected keybinding.
std::vector<int> Action::GetKeyBinds() const
{
	return keybinds;
};

int Action::GetKeyBind(int index) const
{
	return keybinds[index];
};

// Checks if the set key has been pressed.
void Action::SetStrength(int glfwAction)
{
	strength = (glfwAction == GLFW_RELEASE) ? 0 : 1;
};

// Reassigns the keybinding.
void Action::AddKeyBind(int newKey)
{
	keybinds.push_back(newKey);
};

// Returns the name of the action.
const std::string Action::GetActionName() const
{
	return actionName;
};

const char* Action::GetKeyName(int keyCode)
{
	const char* getName = glfwGetKeyName(keybinds[keyCode], 0);

	if (getName)
	{
		//return "	A";
		return getName;
	}

	switch (keybinds[keyCode])
	{
		case GLFW_KEY_LEFT_SHIFT:
			return "	Left-Shift";
		case GLFW_KEY_LEFT:
			return "	Left";
		case GLFW_KEY_RIGHT:
			return "	Right";
		case GLFW_KEY_UP:
			return "	Up";
		case GLFW_KEY_DOWN:
			return "	Down";
		case GLFW_KEY_SPACE:
			return "	Space";
		default:
			return "	Unknown";
	}
}
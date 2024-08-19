#include "Core/action.h"

Action::Action(const std::string actionName, int keyBind)
	: actionName(actionName), strength(0)
{
	AddKeyBind(keyBind);
};

// Returns the result of the key pressed event given from 'SetStrength()'.
int Action::GetStrength() const
{
	return strength;
};

// Returns all keybinds.
std::vector<int> Action::GetKeyBinds() const
{
	return keybinds;
};

// Returns the keybind value using the index.
int Action::GetKeyBindIndex(int index) const
{
	return keybinds[index];
};

// Checks if the set key has been pressed.
void Action::SetStrength(int glfwAction)
{
	strength = (glfwAction == GLFW_RELEASE) ? 0 : 1;
};

// Adds a new keybinding.
void Action::AddKeyBind(int newKey)
{
	if (std::find(keybinds.begin(), keybinds.end(), newKey) != keybinds.end()) return;

	keybinds.push_back(newKey);
};

// Deletes a keybinding.
void Action::DeleteKeyBind(int index)
{
	keybinds.erase(keybinds.begin() + index);
};

// Returns the name of the action.
const std::string Action::GetActionName() const
{
	return actionName;
};

// Returns the name of the keybind.
const char* Action::GetKeyName(int keyCode)
{
	const char* getName = glfwGetKeyName(keybinds[keyCode], 0);

	if (getName)
	{
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
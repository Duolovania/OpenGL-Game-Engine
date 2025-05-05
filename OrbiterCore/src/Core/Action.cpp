#include "Core/action.h"

// The default constructor.
Action::Action(const std::string actionName)
	: m_actionName(actionName), m_strength(0)
{

};

// Additional constructor. This adds an initial keybind.
Action::Action(const std::string actionName, int keyBind)
	: m_actionName(actionName), m_strength(0)
{
	AddKeyBind(keyBind);
};

// Returns the result of the key pressed event given from 'SetStrength()'.
int Action::GetStrength() const
{
	return m_strength;
};

// Returns all keybinds.
std::vector<int> Action::GetKeyBinds() const
{
	return m_keybinds;
};

// Returns the keybind value using the index.
int Action::GetKeyBindIndex(int index) const
{
	return m_keybinds[index];
};

// Checks if the set key has been pressed.
void Action::SetStrength(int glfwAction)
{
	m_strength = (glfwAction == GLFW_RELEASE) ? 0 : 1;
};

// Adds a new keybind.
void Action::AddKeyBind(int newKey)
{
	if (std::find(m_keybinds.begin(), m_keybinds.end(), newKey) != m_keybinds.end()) return;

	m_keybinds.push_back(newKey);
};

// Deletes a keybinding.
void Action::DeleteKeyBind(int index)
{
	m_keybinds.erase(m_keybinds.begin() + index);
};

// Returns the name of the action.
const std::string Action::GetActionName() const
{
	return m_actionName;
};

// Returns the name of the keybind.
const char* Action::GetKeyName(int keyCode)
{
	const char* getName = glfwGetKeyName(m_keybinds[keyCode], 0);

	if (getName)
	{
		return getName;
	}

	switch (m_keybinds[keyCode])
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
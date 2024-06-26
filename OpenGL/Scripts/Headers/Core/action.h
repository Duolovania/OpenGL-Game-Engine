#pragma once
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

class Action
{
	public:
		Action(const std::string actionName = "", int keyBind = 0);
		int GetStrength() const;

		std::vector<int> GetKeyBinds() const;
		const char* GetKeyName(int keyCode);

		int GetKeyBindIndex(int index) const;

		void AddKeyBind(int newKey);
		void DeleteKeyBind(int index);

		void SetStrength(int glfwAction);
		const std::string GetActionName() const;

		// Overwrites the '==' operator for objects of type: Action.
		bool operator==(const Action& other)
		{
			return (other.actionName == this->actionName && other.GetKeyBinds() == this->GetKeyBinds()) ? true : false;
		}

		Action& operator=(const Action& other)
		{
			actionName = other.GetActionName();
			keybinds = other.GetKeyBinds();

			return *this;
		}

	private:
		std::string actionName;
		int strength;

		std::vector<int> keybinds;
};
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

		int GetKeyBind(int index) const;

		void AddKeyBind(int newKey);
		void DeleteKeyBind(int index);

		void SetStrength(int glfwAction);
		const std::string GetActionName() const;

	private:
		const std::string actionName;
		int keybind;
		int strength;

		std::vector<int> keybinds;
};
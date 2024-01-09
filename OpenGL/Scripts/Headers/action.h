#pragma once
#include <glfw3.h>
#include <iostream>

class Action
{
	public:
		Action(const std::string actionName = "");
		int GetStrength() const;
		void SetStrength(int glfwAction);
		const std::string GetActionName() const;

	private:
		const std::string actionName;

		int strength;
};
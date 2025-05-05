#pragma once
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

// This class stores and handles input action data.
class Action
{
	public:
		/**
		* @brief The default constructor.
		*
		* @param actionName the name of the input action.
		*/
		Action(const std::string actionName);

		/**
		* @brief Additional constructor. This adds an initial keybind.
		* 
		* @param actionName the name of the input action.
		* @param keyBind the key code.
		*/
		Action(const std::string actionName, int keyBind);

		/**
		* @brief Gets the result of the key pressed event given from 'SetStrength().'
		*
		* @return the key code.
		*/
		int GetStrength() const;

		/**
		* @brief Gets all keybinds.
		*
		* @return all relevant key codes.
		*/
		std::vector<int> GetKeyBinds() const;

		/**
		* @brief Gets the name of the keybind.
		* 
		* @param keyCode the key code.
		* @return the name.
		*/
		const char* GetKeyName(int keyCode);

		/**
		* @brief Gets the key code using the index.
		* 
		* @param index the index of the keybind.
		* @return the key code.
		*/
		int GetKeyBindIndex(int index) const;

		/**
		* @brief Adds a keybind.
		* 
		* @param newKey the keybind.
		*/
		void AddKeyBind(int newKey);

		/**
		* @brief Deletes a keybind.
		*
		* @param index the index of the keybind.
		*/
		void DeleteKeyBind(int index);

		/**
		* @brief Checks if the key has been pressed.
		* 
		* @param glfwAction the key code.
		*/
		void SetStrength(int glfwAction);

		/**
		* @brief Gets the name of the input action.
		* 
		* @return the name.
		*/
		const std::string GetActionName() const;

		// Overwrites the '==' operator for objects of type: Action. This allows for actions to be compared based on their action name and list of keybinds.
		bool operator==(const Action& other)
		{
			return (other.m_actionName == this->m_actionName && other.GetKeyBinds() == this->GetKeyBinds()) ? true : false;
		}

		// Overwrites the '=' operator for objects of type: Action. This allows for actions to copy action names and keybinds easily.
		Action& operator=(const Action& other)
		{
			m_actionName = other.GetActionName();
			m_keybinds = other.GetKeyBinds();

			return *this;
		}
	private:
		std::string m_actionName;
		int m_strength;

		std::vector<int> m_keybinds;
};
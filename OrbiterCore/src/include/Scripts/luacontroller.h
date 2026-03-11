#pragma once
#include <iostream>
#include "sol/sol.hpp"
#include <vector>
#include "GameObjects/gameobject.h"
#include "Scripts/script.h"

// This class is used for triggering and managing all lua game scripts.
class LuaController
{
	public:
		/**
		* @brief Sets up the controller.
		*/
		void Init();

		/**
		* @brief Adds a script to the controller. This must be called for a script to run.
		* 
		* @param filePath the path to the script. This must include the file extension.
		*/
		void AddScript(std::string filePath);

		void BindGameObject(std::vector<Script> scripts, GameObject gObj);

		///**
		//* @brief Makes a C++ method usable in scripts.
		//* 
		//* @param functionName the name of the function. This is how it will be called in the script.
		//* @param function the C++ function that will be bound.
		//*/
		//void AddFunction(std::string functionName, []const std::string& name)
		//{
		//	m_lua.set_function(functionName, &name); // Adds 'TestMethod' C++ method to lua as 'HelloAll'.
		//}

		/**
		* @brief Runs 'Update' methods from all loaded scripts.
		*/
		void CallUpdate() const;

		/**
		* @brief Runs 'Start' methods from all loaded scripts.
		*/
		void CallStart() const;

		sol::state m_lua;
	private:
		std::vector<sol::function> m_updateFunctions;
		std::vector<sol::function> m_startFunctions;
};
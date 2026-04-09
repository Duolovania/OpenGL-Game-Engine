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

		sol::state m_lua;
};
#pragma once
#include <string>
#include "sol/sol.hpp"

// This class stores the script details.
class Script
{
	public:
		/**
		* @brief The default constructor.
		* 
		*/
		Script();
		Script(sol::state& luaController, std::string filePath);

		/*Script operator=(const Script& s)
		{
			Script newScript = Script(s);
		}*/

		void SetPath(std::string filePath);
		void Gen(sol::state& luaController);

		std::string GetPath() const;
		sol::table GetLua() const;
	private:
		// The path where the script file is stored.
		std::string m_path;
		sol::table m_scriptFile;
};
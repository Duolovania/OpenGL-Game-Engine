#pragma once
#include <vector>
#include "Components/component.h"
#include "Scripts/script.h"
#include "GameObjects/gameobject.h"

class ScriptManager : public Component
{
	public:
		ScriptManager();
		~ScriptManager();

		void UnbindScript(std::string scriptPath);
		void BindScript(sol::state& luaController, std::string scriptPath);
		void BindGameObjects(sol::state& luaController, GameObject gObj);

		/**
		* @brief Runs 'Update' methods from all loaded scripts.
		*/
		void CallUpdate() const;

		/**
		* @brief Runs 'Start' methods from all loaded scripts.
		*/
		void CallStart() const;

		std::vector<Script> GetScripts() const;
		void SetScripts(std::vector<Script> scripts);
	private:
		std::vector<Script> m_scripts;
};
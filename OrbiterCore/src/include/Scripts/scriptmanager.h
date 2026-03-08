#pragma once
#include <vector>
#include "Components/component.h"
#include "Scripts/script.h"

class ScriptManager : public Component
{
	public:
		ScriptManager();
		~ScriptManager();

		void UnbindScript(std::string scriptPath);
		void BindScript(std::string scriptPath);

		std::vector<Script> GetScripts() const;
		void SetScripts(std::vector<Script> scripts);
	private:
		std::vector<Script> m_scripts;
};
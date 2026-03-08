#include "Scripts/scriptmanager.h"

ScriptManager::ScriptManager()
{
	m_componentName = "Script Manager";
}

ScriptManager::~ScriptManager()
{
}

void ScriptManager::UnbindScript(std::string scriptPath)
{
	for (int i = 0; i < m_scripts.size(); i++)
	{
		if (m_scripts[i].GetPath() == scriptPath) m_scripts.erase(m_scripts.begin() + i); // Removes script from vector.
	}
}

void ScriptManager::BindScript(std::string scriptPath)
{
	Script newScript;
	newScript.SetPath(scriptPath);
	m_scripts.push_back(newScript); // Adds script to vector.
}

std::vector<Script> ScriptManager::GetScripts() const
{
	return m_scripts;
}

void ScriptManager::SetScripts(std::vector<Script> scripts)
{
	m_scripts = scripts;
}

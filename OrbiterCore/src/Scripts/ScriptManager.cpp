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

void ScriptManager::BindScript(sol::state& luaController, std::string scriptPath)
{
	Script newScript = Script(luaController, scriptPath);
	m_scripts.push_back(newScript); // Adds script to vector.
}

void ScriptManager::BindGameObjects(sol::state& luaController, GameObject gObj)
{
	for (int i = 0; i < m_scripts.size(); i++)
	{
		m_scripts[i].Gen(luaController);
		m_scripts[i].GetLua()["gameobject"] = gObj;
		m_scripts[i].GetLua()["transform"] = &gObj.transform;
	}
}

void ScriptManager::CallUpdate() const
{
	for (auto& script : m_scripts)
	{
		sol::function updateFunc = script.GetLua()["Update"];
		if (updateFunc.valid()) updateFunc(script.GetLua());
	}
}

void ScriptManager::CallStart() const
{
	for (auto& script : m_scripts)
	{
		sol::function startFunc = script.GetLua()["Start"];
		if (startFunc.valid()) startFunc(script.GetLua());
	}
}

std::vector<Script> ScriptManager::GetScripts() const
{
	return m_scripts;
}

void ScriptManager::SetScripts(std::vector<Script> scripts)
{
	m_scripts = scripts;
}

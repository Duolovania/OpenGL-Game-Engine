#include "Scripts/script.h"

Script::Script()
{
}

Script::Script(sol::state& luaController, std::string filePath)
{
	m_path = filePath;
	Gen(luaController);
}

void Script::SetPath(std::string filePath)
{
	m_path = filePath;
}

void Script::Gen(sol::state& luaController)
{
	sol::table baseScript = luaController.script_file(m_path);
	m_scriptFile = luaController.create_table();

	sol::table mt = luaController.create_table();
	mt["__index"] = baseScript;

	m_scriptFile[sol::metatable_key] = mt;
}

std::string Script::GetPath() const
{
	return m_path;
}

sol::table Script::GetLua() const
{
	return m_scriptFile;
}

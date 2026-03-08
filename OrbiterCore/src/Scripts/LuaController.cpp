#include "Scripts/luacontroller.h"
#include "Core/debug.h"
#include "GameObjects/gameobject.h"

void LuaController::Init()
{
    m_lua.open_libraries(sol::lib::base, sol::lib::math);

    // Adds console logging capabilities to scripts.
    m_lua.set_function("LogOB", [](sol::object obj) 
    {
        if (obj.is<std::string>()) 
        {
            DebugOB.Log(obj.as<std::string>());
        }
        else if (obj.is<int>()) 
        {
            DebugOB.Log(std::to_string(obj.as<int>()));
        }
        else 
        {
            DebugOB.Log("Unknown type passed to Log");
        }
    });
}

void LuaController::AddScript(std::string filePath)
{
    try
    {
        std::cout << filePath << std::endl;
        m_lua.script_file(filePath); // Loads lua script file under the script table.

        // Adds the start and update methods to vectors.
        m_updateFunctions.push_back(m_lua["Update"]);
        m_startFunctions.push_back(m_lua["Start"]);
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void LuaController::CallUpdate() const
{
    for (auto& update : m_updateFunctions)
    {
        update();
    }
}

void LuaController::CallStart() const
{
    for (auto& start : m_startFunctions)
    {
        start();
    }
}

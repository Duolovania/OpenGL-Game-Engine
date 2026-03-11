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

    // Expose cpp classes to lua scripts.
    m_lua.new_usertype<Transform>("Transform", "position", &Transform::position, "rotation", &Transform::rotation, "scale", &Transform::scale);
    m_lua.new_usertype<glm::vec3>("Vector3", "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
    m_lua.new_usertype<glm::vec2>("Vector2", "x", &glm::vec3::x, "y", &glm::vec3::y);
    m_lua.new_usertype<GameObject>("GameObject", "transform", &GameObject::GetComponent<Transform>);

    // TODO: add sprite renderer and camera.
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

void LuaController::BindGameObject(std::vector<Script> scripts, GameObject gObj)
{
    for (auto s : scripts)
    {
        sol::table script = m_lua.script_file(s.GetPath());
        script["self"] = gObj;
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

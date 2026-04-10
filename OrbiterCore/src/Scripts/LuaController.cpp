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
        else if (obj.is<float>())
        {
            DebugOB.Log(std::to_string(obj.as<float>()));
        }
        else if (obj.is<bool>())
        {
            DebugOB.Log(std::to_string(obj.as<bool>()));
        }
        else 
        {
            DebugOB.Log("Unknown type passed to Log");
        }
    });

    // Expose cpp classes to lua scripts.
    m_lua.new_usertype<glm::vec3>("Vector3", "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
    m_lua.new_usertype<glm::vec2>("Vector2", "x", &glm::vec2::x, "y", &glm::vec2::y);
    m_lua.new_usertype<Transform>("Transform", "position", &Transform::position, "rotation", &Transform::rotation, "scale", &Transform::scale);
    m_lua.new_usertype<GameObject>("GameObject", "transform", &GameObject::GetComponent<Transform>, "name", &GameObject::objectName);

    // TODO: add sprite renderer and camera.
}
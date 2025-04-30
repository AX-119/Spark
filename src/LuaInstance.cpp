#include "LuaInstance.h"
#include <Component.h>
#include <glm/glm.hpp>
#include <Components/TransformComponent.h>
#include <Components/ScriptComponent.h>

namespace spark
{
    void LuaInstance::Init()
    {
        m_Lua.open_libraries(sol::lib::base, sol::lib::package);
        SetupBindings();
    }
    void LuaInstance::SetupBindings()
    {

        // Set the global lua space variables
        // (this tells the lua space e.g. what a "GameObject" is and what functions it can call)
        // format: "NameInLua", functionptr
        m_Lua.new_usertype<spark::Component>("Component", sol::no_constructor, "GetParent", &spark::Component::GetParent);

        m_Lua.new_usertype<glm::vec3>(
            "vec3",
            sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b)
            { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b)
            { return a - b; },
            sol::meta_function::multiplication, sol::overload([](const glm::vec3 &v, float s)
                                                              { return v * s; }, [](float s, const glm::vec3 &v)
                                                              { return s * v; }),
            sol::meta_function::division, [](const glm::vec3 &v, float s)
            { return v / s; },
            "length", [](const glm::vec3 &v)
            { return glm::length(v); },
            "normalize", [](const glm::vec3 &v)
            { return glm::normalize(v); },
            sol::meta_function::to_string, [](const glm::vec3 &v)
            { return "{ x= " + std::to_string(v.x) + ", y= " + std::to_string(v.y) + ", z= " + std::to_string(v.z) + " }"; });

        m_Lua.new_usertype<glm::quat>(
            "quat", sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),
            "x", &glm::quat::x,
            "y", &glm::quat::y,
            "z", &glm::quat::z,
            "w", &glm::quat::w,
            sol::meta_function::to_string, [](const glm::quat &q)
            { return "{ w=" + std::to_string(q.w) + "x= " + std::to_string(q.x) + ", y= " + std::to_string(q.y) + ", z= " + std::to_string(q.z) + " }"; },
            sol::meta_function::multiplication, [](const glm::quat &a, const glm::quat &b)
            { return a * b; });

        m_Lua.new_usertype<spark::TransformComponent>("TransformComponent", sol::no_constructor, sol::base_classes, sol::bases<spark::Component>(),
                                                      "SetLocalPosition", &spark::TransformComponent::SetLocalPosition,
                                                      "GetLocalPosition", &spark::TransformComponent::GetLocalPosition,
                                                      "SetLocalRotation", &spark::TransformComponent::SetLocalRotation,
                                                      "GetLocalRotation", &spark::TransformComponent::GetLocalRotation,
                                                      "SetLocalScale", &spark::TransformComponent::SetLocalScale,
                                                      "GetLocalScale", &spark::TransformComponent::GetLocalScale,
                                                      "GetWorldPosition", &spark::TransformComponent::GetWorldPosition,
                                                      "GetWorldRotation", &spark::TransformComponent::GetWorldRotation,
                                                      "GetWorldScale", &spark::TransformComponent::GetWorldScale,
                                                      "GetWorldMatrix", &spark::TransformComponent::GetWorldMatrix);

        m_Lua.new_usertype<spark::ScriptComponent>("ScriptComponent", sol::no_constructor, sol::base_classes, sol::bases<spark::Component>(),
                                                   "ReloadScript", &spark::ScriptComponent::ReloadScript);
        m_Lua.new_usertype<spark::GameObject>("GameObject", sol::no_constructor, "GetParent", &spark::GameObject::GetParent);
    }
} // namespace spark

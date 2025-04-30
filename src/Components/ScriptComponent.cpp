#include "Components/ScriptComponent.h"
#include "LuaInstance.h"
namespace spark
{
    ScriptComponent::ScriptComponent(GameObject *parent, const std::string &scriptPath) : Component(parent),
                                                                                          m_scriptPath{scriptPath},
                                                                                          m_scriptEnv{LuaInstance::GetInstance().GetState(), sol::create, LuaInstance::GetInstance().GetState().globals()}
    {
    }
    void ScriptComponent::Init()
    {
        m_scriptEnv["gameObject"] = GetParent();
        if (LoadAndExecuteScript())
        {
            m_luaInit = m_scriptEnv["Init"];
            m_luaUpdate = m_scriptEnv["Update"];
            m_luaRender = m_scriptEnv["Render"];
            m_luaImGuiRender = m_scriptEnv["RenderImGui"];

            if (!m_luaInit.valid() || m_luaInit.get_type() != sol::type::function)
                std::cerr << "Couldn't find \'Init()\' in lua script!\n";
            if (!m_luaUpdate.valid() || m_luaUpdate.get_type() != sol::type::function)
                std::cerr << "Couldn't find \'Update()\' in lua script!\n";
            if (!m_luaRender.valid() || m_luaRender.get_type() != sol::type::function)
                std::cerr << "Couldn't find \'Render()\' in lua script!\n";
            if (!m_luaImGuiRender.valid() || m_luaImGuiRender.get_type() != sol::type::function)
                std::cerr << "Couldn't find \'RenderImGui()\' in lua script!\n";
        }
    }
    bool ScriptComponent::LoadAndExecuteScript()
    {
        auto &lua = LuaInstance::GetInstance().GetState();
        auto result = lua.safe_script_file(m_scriptPath, m_scriptEnv, sol::script_pass_on_error);
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error loading script! Path [ " << m_scriptPath << " ] : " << error.what() << "\n";
            return false;
        }
        std::cerr << "Script loaded successfully! Path [ " << m_scriptPath << " ]\n";
        return true;
    }
    void ScriptComponent::Update()
    {
        if (!m_luaUpdate.valid() || m_luaUpdate.get_type() != sol::type::function)
            std::cerr << "Couldn't find \'Update()\' in lua script!\n";
        auto result = m_luaUpdate();
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error in Lua script Update(): " << error.what() << "\n";
        }
    }
    void ScriptComponent::Render()
    {
        if (!m_luaRender.valid() || m_luaRender.get_type() != sol::type::function)
            std::cerr << "Couldn't find \'Render()\' in lua script!\n";
        auto result = m_luaRender();
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error in Lua script Render(): " << error.what() << "\n";
        }
    }
    void ScriptComponent::RenderImGui()
    {
        if (!m_luaImGuiRender.valid() || m_luaImGuiRender.get_type() != sol::type::function)
            std::cerr << "Couldn't find \'RenderImGui()\' in lua script!\n";
        auto result = m_luaImGuiRender();
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error in Lua script RenderImGui():" << error.what() << "\n";
        }
    }
    bool ScriptComponent::ReloadScript()
    {
        return true;
    }
} // namespace spark

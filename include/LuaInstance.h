#ifndef LUAINSTANCE_H
#define LUAINSTANCE_H

#include "Singleton.h"
#include <sol/sol.hpp>
namespace spark
{
    class LuaInstance final : public Singleton<LuaInstance>
    {
    public:
        LuaInstance(const LuaInstance &other) = delete;
        LuaInstance(LuaInstance &&other) = delete;
        LuaInstance &operator=(const LuaInstance &other) = delete;
        LuaInstance &operator=(LuaInstance &&other) = delete;

        void Init();
        sol::state &GetState() { return m_Lua; }

    private:
        friend Singleton<LuaInstance>;
        LuaInstance() = default;
        void SetupBindings();
        sol::state m_Lua;
    };
} // namespace spark

#endif // LUAINSTANCE_H

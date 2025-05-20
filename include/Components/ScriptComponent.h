#ifndef SCRIPTCOMPONENT_H
#define SCRIPTCOMPONENT_H

#include "Component.h"
#include "IUpdateable.h"
#include "IRenderable.h"
#include "IImGuiRenderable.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace spark
{
    class ScriptComponent final : public Component, public IUpdateable, public IRenderable, public IImGuiRenderable
    {
    public:
        explicit ScriptComponent(GameObject *parent, const std::string &scriptPath);
        virtual ~ScriptComponent() = default;

        void Init();

        void Update(float dt) override;
        void Render() override;
        void RenderImGui() override;
        void Test();
        bool ReloadScript();

    private:
        std::string m_scriptPath{};
        sol::environment m_scriptEnv;

        bool LoadAndExecuteScript();
        bool m_hasInitFunction{true};
        bool m_hasUpdateFunction{true};
        bool m_hasRenderFunction{true};
        bool m_hasRenderImGuiFunction{true};

        sol::protected_function m_luaUpdate;
        sol::protected_function m_luaRender;
        sol::protected_function m_luaImGuiRender;
        sol::protected_function m_luaInit;
    };
}

#endif // SCRIPTCOMPONENT_H

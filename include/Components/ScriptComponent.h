#pragma once
#include "Component.h"
#include <sol/sol.hpp>
#include <string>
#include "imgui.h"
#include "IUpdateable.h"
#include "IRenderable.h"
#include "IInitializable.h"
#include "IImGuiRenderable.h"
#include "IInspectorRenderable.h"
#include "TextEditor.h"

namespace spark
{
    class ScriptComponent : public Component, public IInitializable, public IUpdateable, public IRenderable, public IImGuiRenderable, public IInspectorRenderable
    {
    public:
        ScriptComponent(GameObject *parent, const std::string &scriptPath);

        void Init() override;
        void Update(float dt) override;
        void Render() override;
        void RenderImGui() override;
        void RenderInspector() override;

        bool ReloadScript();

        void AddScript();
        bool LoadScriptContent();
        bool SaveScriptContent();
        void RenderScriptEditor();
        int CountLines() const;

        static int TextEditCallback(ImGuiInputTextCallbackData *data);

    private:
        bool LoadAndExecuteScript();

    private:
        std::string m_scriptPath;
        std::string m_scriptContent;
        sol::environment m_scriptEnv;

        sol::protected_function m_luaInit;
        sol::protected_function m_luaUpdate;
        sol::protected_function m_luaRender;
        sol::protected_function m_luaImGuiRender;

        bool m_hasInitFunction = true;
        bool m_hasUpdateFunction = true;
        bool m_hasRenderFunction = true;
        bool m_hasRenderImGuiFunction = true;

        bool m_isEditorOpen = false;
        bool m_hasUnsavedChanges = false;
    };

}
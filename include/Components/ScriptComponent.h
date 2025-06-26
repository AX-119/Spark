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
        const std::string &GetScriptPath() const { return m_scriptPath; }
        void SetScriptPath(const std::string &scriptPath) { m_scriptPath = scriptPath; }
        void ClearScriptContent();
        void AddScript();
        bool LoadScriptContent();
        bool SaveScriptContent();
        void RenderScriptEditor();
        void RenderToolbar();
        void HandleKeyboardShortcuts();
        void RenderConfirmationPopups();
        void RenderTextEditor();
        void MarkAsModified();
        void RenderFindReplace();
        void HandlePasteOperation();
        int CountLines() const;

        static int TextEditCallback(ImGuiInputTextCallbackData *data);

    private:
        bool LoadAndExecuteScript();

    private:
        std::string m_scriptPath;
        std::string m_scriptContent;
        std::string m_selectedText;
        sol::environment m_scriptEnv;
        bool m_requestedPaste = false;
        int m_cursorPos{};

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

        struct
        {
            bool save = false;
            bool copy = false;
            bool paste = false;
            bool undo = false;
            bool redo = false;
        } m_shortcuts;

        bool m_showFindReplace = false;
        bool m_focusFindInput = false;
        bool m_matchCase = false;
        bool m_wholeWord = false;
        char m_findText[256] = "";
        char m_replaceText[256] = "";

        // bool CanUndo();
        // bool CanRedo();
        // void Undo();
        // void Redo();
        // void PushUndoState();
        // void FindNext();
        // void FindPrevious();
        // void Replace();
        // void ReplaceAll();
        // std::string GetLineEndingString();
    };

}
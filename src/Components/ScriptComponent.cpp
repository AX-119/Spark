#include "Components/ScriptComponent.h"
#include "LuaInstance.h"
#include "imgui.h"
#include <fstream>
#include <sstream>
#include <filesystem>

#ifdef __EMSCRIPTEN__
#include "emscripten_browser_clipboard.h"
#include "EditorUI.h"
#endif

namespace spark
{
    ScriptComponent::ScriptComponent(GameObject *parent, const std::string &scriptPath) : Component(parent),
                                                                                          m_scriptPath{scriptPath},
                                                                                          m_scriptEnv{LuaInstance::GetInstance().GetState(), sol::create, LuaInstance::GetInstance().GetState().globals()},
                                                                                          m_scriptContent{},
                                                                                          m_isEditorOpen{false},
                                                                                          m_hasUnsavedChanges{false}
    {
        LoadScriptContent();
    }

    void ScriptComponent::Init()
    {
        m_scriptEnv["gameObject"] = GetParent();

        m_hasInitFunction = false;
        m_hasUpdateFunction = false;
        m_hasRenderFunction = false;
        m_hasRenderImGuiFunction = false;

        m_luaInit = sol::nil;
        m_luaUpdate = sol::nil;
        m_luaRender = sol::nil;
        m_luaImGuiRender = sol::nil;

        if (LoadAndExecuteScript())
        {
            m_luaInit = m_scriptEnv["Init"];
            m_luaUpdate = m_scriptEnv["Update"];
            m_luaRender = m_scriptEnv["Render"];
            m_luaImGuiRender = m_scriptEnv["RenderImGui"];

            sol::object luaInitObj = m_scriptEnv["Init"];
            if (luaInitObj.is<sol::function>())
            {
                m_luaInit = luaInitObj.as<sol::protected_function>();
                m_hasInitFunction = true;
            }
            else if (luaInitObj.valid())
            {
                std::cerr << "Script '" << m_scriptPath << "': 'Init' found but is not a function.\n";
            }

            sol::object luaUpdateObj = m_scriptEnv["Update"];
            if (luaUpdateObj.is<sol::function>())
            {
                m_luaUpdate = luaUpdateObj.as<sol::protected_function>();
                m_hasUpdateFunction = true;
            }
            else if (luaUpdateObj.valid())
            {
                std::cerr << "Script '" << m_scriptPath << "': 'Update' found but is not a function.\n";
            }

            sol::object luaRenderObj = m_scriptEnv["Render"];
            if (luaRenderObj.is<sol::function>())
            {
                m_luaRender = luaRenderObj.as<sol::protected_function>();
                m_hasRenderFunction = true;
            }
            else if (luaRenderObj.valid())
            {
                std::cerr << "Script '" << m_scriptPath << "': 'Render' found but is not a function.\n";
            }

            sol::object luaImGuiRenderObj = m_scriptEnv["RenderImGui"];
            if (luaImGuiRenderObj.is<sol::function>())
            {
                m_luaImGuiRender = luaImGuiRenderObj.as<sol::protected_function>();
                m_hasRenderImGuiFunction = true;
            }
            else if (luaImGuiRenderObj.valid())
            {
                std::cerr << "Script '" << m_scriptPath << "': 'RenderImGui' found but is not a function.\n";
            }
        }
        else
        {
            std::cerr << "ScriptComponent::Init: Due to script execution failure for '" << m_scriptPath
                      << "', all Lua functions are marked unavailable.\n";
        }
    }

    bool ScriptComponent::LoadScriptContent()
    {
        if (m_scriptPath.empty())
        {
            std::cerr << "Failed to load script content: Script path is empty.\n";
            m_scriptContent.clear();
            m_hasUnsavedChanges = false;
            return false;
        }

        std::ifstream file(m_scriptPath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open script file: " << m_scriptPath << "\n";
            m_scriptContent.clear();
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        m_scriptContent = buffer.str();
        file.close();

        m_hasUnsavedChanges = false;
        return true;
    }

    bool ScriptComponent::SaveScriptContent()
    {

        if (m_scriptPath.empty())
        {
            std::cerr << "ERROR: Script path is empty. Cannot save." << std::endl;
            return false;
        }

        std::ofstream file(m_scriptPath);
        if (!file.is_open())
        {
            std::cerr << "ERROR: Failed to open file for saving at '" << m_scriptPath << "'. Check path validity and permissions." << std::endl;
            return false;
        }
        file << m_scriptContent;

        if (file.fail())
        {
            std::cerr << "ERROR: stream.fail() is true after writing content to '" << m_scriptPath << "' (before close)." << std::endl;
            file.close();
            return false;
        }

        file.close();

        if (file.fail())
        {
            std::cerr << "ERROR: stream.fail() is true after closing file '" << m_scriptPath << "'." << std::endl;
            return false;
        }

        m_hasUnsavedChanges = false;
        return true;
    }

    bool ScriptComponent::LoadAndExecuteScript()
    {
        auto &lua = LuaInstance::GetInstance().GetState();

        if (m_scriptContent.empty())
        {
            if (!m_scriptPath.empty())
            {
                std::cerr << "Warning: Script content for '" << m_scriptPath << "' is empty at execution time. "
                          << "Ensure LoadScriptContent() was called and succeeded.\n";
            }
            else
            {
                std::cerr << "Error: No script path and script content is empty. Cannot execute.\n";
            }
            return false;
        }

        auto result = lua.safe_script(m_scriptContent, m_scriptEnv);
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error loading script! Path [ " << m_scriptPath << " ] : " << error.what() << "\n";
            return false;
        }
        std::cerr << "Script loaded successfully! Path [ " << m_scriptPath << " ]\n";
        return true;
    }

    void ScriptComponent::Update(float dt)
    {
        if (!m_hasUpdateFunction)
        {
            return;
        }
        auto result = m_luaUpdate(dt);
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error in Lua script Update(): " << error.what() << "\n";
        }
    }

    void ScriptComponent::Render()
    {
        if (!m_hasRenderFunction)
        {
            return;
        }
        auto result = m_luaRender();
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error in Lua script Render(): " << error.what() << "\n";
        }
    }

    void ScriptComponent::RenderImGui()
    {
        if (!m_hasRenderImGuiFunction)
        {
            return;
        }
        auto result = m_luaImGuiRender();
        if (!result.valid())
        {
            sol::error error = result;
            std::cerr << "Error in Lua script RenderImGui():" << error.what() << "\n";
        }
    }

    void ScriptComponent::RenderInspector()
    {
        if (ImGui::CollapsingHeader("Script Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (m_hasUnsavedChanges)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Path: %s *", m_scriptPath.c_str());
            }
            else
            {
                ImGui::Text("Path: %s", m_scriptPath.c_str());
            }
            ImGui::Text("Script Content Lines: %d", CountLines());

            // Button row
            if (ImGui::Button("Add Script"))
            {
                ImGui::OpenPopup("New Script##AddScript");
            }
            ImGui::SameLine();
            if (ImGui::Button("Reload Script"))
            {
                if (m_hasUnsavedChanges)
                {
                    ImGui::OpenPopup("Confirm Reload Unsaved Changes##Inspector");
                }
                else
                {
                    ReloadScript();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Edit Script"))
            {
                if (m_scriptPath.empty() && m_scriptContent.empty())
                {
                    std::cerr << "Cannot edit script: No script path specified and no content loaded." << std::endl;
                }
                else if (m_scriptContent.empty() && !m_scriptPath.empty() && !LoadScriptContent())
                {
                    std::cerr << "Cannot edit script: Failed to load content from " << m_scriptPath << std::endl;
                }
                else
                {
                    m_isEditorOpen = true;
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Save Script"))
            {
                if (m_hasUnsavedChanges)
                {
                    SaveScriptContent();
                }
                else
                {
                    ImGui::Text("No unsaved changes.");
                }
            }

            // Unsaved changes confirmation popup for Reload Script
            if (ImGui::BeginPopupModal("Confirm Reload Unsaved Changes##Inspector", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("You have unsaved changes in the editor for '%s'.", m_scriptPath.c_str());
                ImGui::Text("Reloading from file will discard these changes.");
                ImGui::Text("Are you sure you want to continue?");
                ImGui::Separator();

                if (ImGui::Button("Yes, Discard & Reload", ImVec2(180, 0)))
                {
                    ReloadScript();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::Separator();
        }

        // Script Editor Window
        if (m_isEditorOpen)
        {
            RenderScriptEditor();
        }
        AddScript();
    }

    void ScriptComponent::RenderScriptEditor()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        std::string persistentId = "###ScriptEditor_" + m_scriptPath;
        std::string displayedTitle = "Script Editor - " + m_scriptPath;
        if (m_hasUnsavedChanges)
        {
            displayedTitle += " *";
        }
        std::string windowIdentifier = displayedTitle + persistentId;
        if (ImGui::Begin(windowIdentifier.c_str(), &m_isEditorOpen))
        {
            // Toolbar
            if (ImGui::Button("Save"))
            {
                SaveScriptContent();
            }

            ImGui::SameLine();
            if (ImGui::Button("Save & Reload"))
            {
                if (SaveScriptContent())
                {
                    ReloadScript();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Reload from File"))
            {
                if (m_hasUnsavedChanges)
                {
                    ImGui::OpenPopup("Confirm Discard Editor Changes##EditorReload");
                }
                else
                {
                    if (LoadScriptContent())
                    {
                        ReloadScript();
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Copy"))
            {
                if (!m_selectedText.empty())
                {
                    std::cout << "SetClipboardText: " << m_selectedText << "\n";
#ifndef __EMSCRIPTEN__
                    ImGui::SetClipboardText(m_selectedText.c_str());
#else
                    SetClipboardFromImGui(nullptr, m_selectedText.c_str());
#endif
                }
            }
            bool textEditFocused = false;
            ImGui::SameLine();
            if (ImGui::Button("Paste"))
            {
                m_requestedPaste = true;
            }
            ImGui::SameLine();
            ImGui::Text("Lines: %d", CountLines());

            // Keyboard shortcuts
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
            {
                if (SaveScriptContent())
                {
                    // Potentially give feedback
                }
            }
#ifdef __EMSCRIPTEN__
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C, false))
            {
                SetClipboardFromImGui(nullptr, m_selectedText.c_str());
            }
#endif

            // Unsaved changes confirmation popup for editor
            if (ImGui::BeginPopupModal("Confirm Discard Editor Changes##EditorReload", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("You have unsaved changes. Reloading from file will discard them.");
                ImGui::Text("Are you sure you want to continue?");
                ImGui::Separator();
                if (ImGui::Button("Yes, Discard Changes", ImVec2(150, 0)))
                {
                    LoadScriptContent(); // Load from file, discarding editor changes
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(150, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::Separator();

            ImVec2 editorSize = ImGui::GetContentRegionAvail();
            editorSize.y -= ImGui::GetStyle().ScrollbarSize;

            // Reserve some space if it's very small to avoid frequent reallocations at start.
            if (m_scriptContent.capacity() < 1024)
            {
                m_scriptContent.reserve(1024);
            }

            // Note: ImGui::InputTextMultiline wants a non-const char*.
            // std::string::data() (C++17+) or &m_scriptContent[0] (for non-empty strings) can provide this.
            // Need to ensure the string's internal buffer is large enough.
            // The TextEditCallback handles resizing m_scriptContent.
            if (ImGui::InputTextMultiline("##ScriptEditSource",
                                          m_scriptContent.data(),
                                          m_scriptContent.capacity(),
                                          editorSize,
                                          ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackAlways,
                                          TextEditCallback,
                                          this)) // User data for callback
            {
                // This block is entered when the content is *changed* by the user.
                // The TextEditCallback should have already updated m_scriptContent's size.
                // We just need to mark that there are unsaved changes.
                m_hasUnsavedChanges = true;
            }
            if (m_requestedPaste)
            {
                m_requestedPaste = false;
                const char *clipboardText = ImGui::GetClipboardText();
                if (clipboardText && clipboardText[0] != '\0')
                {
                    // Insert at current cursor position
                    m_scriptContent.insert(m_cursorPos, clipboardText);

                    // Update cursor position
                    m_cursorPos += strlen(clipboardText);

                    // Mark as edited
                    m_hasUnsavedChanges = true;
                }
            }
            ImGui::End();
        }
    }

    int ScriptComponent::TextEditCallback(ImGuiInputTextCallbackData *data)
    {
        ScriptComponent *component = static_cast<ScriptComponent *>(data->UserData);
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            // Resize std::string to the new required size
            IM_ASSERT(data->BufSize > 0);
            component->m_scriptContent.resize(data->BufTextLen); // BufTextLen is length of text BEFORE null terminator
            data->Buf = &component->m_scriptContent[0];          // Update ImGui's buffer pointer to our string's data
        }
        else if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways)
        {
            // Track selection for copy/paste
            if (data->SelectionStart != data->SelectionEnd)
            {
                int start = data->SelectionStart;
                int end = data->SelectionEnd;
                if (start > end)
                    std::swap(start, end);

                component->m_selectedText = std::string(data->Buf + start, end - start);
            }
            else
            {
                component->m_selectedText.clear();
            }

            component->m_cursorPos = data->CursorPos;
        }
        // Can also handle ImGuiInputTextFlags_Edited here if needed, but the return of InputTextMultiline covers it.
        return 0;
    }

    int ScriptComponent::CountLines() const
    {
        return static_cast<int>(std::count(m_scriptContent.begin(), m_scriptContent.end(), '\n') + 1);
    }

    bool ScriptComponent::ReloadScript()
    {
        if (!LoadScriptContent())
        {
            std::cerr << "ReloadScript: Failed to load script content from file. Aborting reload.\n";
            m_hasInitFunction = false;
            m_hasUpdateFunction = false;
            m_hasRenderFunction = false;
            m_hasRenderImGuiFunction = false;
            m_luaInit = sol::nil;
            m_luaUpdate = sol::nil;
            m_luaRender = sol::nil;
            m_luaImGuiRender = sol::nil;
            return false;
        }

        lua_State *L = LuaInstance::GetInstance().GetState();
        m_scriptEnv = sol::environment(L, sol::create, LuaInstance::GetInstance().GetState().globals());

        Init();
        std::cout << "Script reloaded: " << m_scriptPath << std::endl;

        if (m_hasInitFunction && m_luaInit.valid())
        {
            auto result = m_luaInit();
            if (!result.valid())
            {
                sol::error err = result;
                std::cerr << "Error executing Lua script's Init() function for '" << m_scriptPath << "': " << err.what() << "\n";
            }
        }

        return true;
    }

    void ScriptComponent::AddScript()
    {
        static char scriptName[256] = "";
        static std::string errorMessage = "";
        static bool showError = false;

        if (ImGui::BeginPopupModal("New Script##AddScript", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Create a new Lua script:");
            ImGui::Separator();

            ImGui::Text("Script name:");
            ImGui::InputText("##scriptName", scriptName, sizeof(scriptName));

            // Show current path preview
            if (strlen(scriptName) > 0)
            {
                std::string previewPath = "res/scripts/" + std::string(scriptName) + ".lua";
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Path: %s", previewPath.c_str());
            }

            // Show error message if any
            if (showError)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", errorMessage.c_str());
            }

            ImGui::Separator();

            // Buttons
            bool canCreate = strlen(scriptName) > 0;
            if (!canCreate)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Create Script", ImVec2(120, 0)))
            {
                std::string newScriptPath = "res/scripts/" + std::string(scriptName) + ".lua";

                // Check if script already exists
                if (std::filesystem::exists(newScriptPath))
                {
                    errorMessage = "Error: A script with this name already exists.";
                    showError = true;
                }
                else
                {
                    // Ensure the scripts directory exists
                    std::filesystem::create_directories("res/scripts");

                    // Create the new script file with basic template
                    std::ofstream file(newScriptPath);
                    if (file.is_open())
                    {
                        // Write a basic Lua script template
                        file << "-- " << scriptName << ".lua\n";
                        file << "-- Generated script file\n\n";
                        file << "function Init()\n";
                        file << "    -- Initialize your script here\n";
                        file << "    print(\"" << scriptName << " initialized\")\n";
                        file << "end\n\n";
                        file << "function Update(dt)\n";
                        file << "    -- Update logic here (dt = delta time)\n";
                        file << "end\n\n";
                        file << "function Render()\n";
                        file << "    -- Render logic here\n";
                        file << "end\n\n";
                        file << "function RenderImGui()\n";
                        file << "    -- ImGui rendering here\n";
                        file << "end\n";

                        file.close();

                        // Update this component to use the new script
                        m_scriptPath = newScriptPath;

                        // Load and initialize the new script
                        if (LoadScriptContent())
                        {
                            Init(); // Reinitialize with new script
                            std::cout << "New script created and loaded: " << newScriptPath << std::endl;
                        }
                        else
                        {
                            std::cerr << "Failed to load newly created script: " << newScriptPath << std::endl;
                        }

                        // Reset state and close popup
                        memset(scriptName, 0, sizeof(scriptName));
                        showError = false;
                        errorMessage.clear();
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        errorMessage = "Error: Failed to create script file. Check permissions.";
                        showError = true;
                    }
                }
            }

            if (!canCreate)
            {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                // Reset state and close popup
                memset(scriptName, 0, sizeof(scriptName));
                showError = false;
                errorMessage.clear();
                ImGui::CloseCurrentPopup();
            }

            // Clear error when user starts typing
            if (ImGui::IsItemActive() || ImGui::IsItemEdited())
            {
                showError = false;
                errorMessage.clear();
            }

            ImGui::EndPopup();
        }
    }

    void ScriptComponent::ClearScriptContent()
    {
        m_scriptContent.clear();
        m_hasUnsavedChanges = false;

        // Reset all Lua function flags and references
        m_hasInitFunction = false;
        m_hasUpdateFunction = false;
        m_hasRenderFunction = false;
        m_hasRenderImGuiFunction = false;

        m_luaInit = sol::nil;
        m_luaUpdate = sol::nil;
        m_luaRender = sol::nil;
        m_luaImGuiRender = sol::nil;
    }

} // namespace spark
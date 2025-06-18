#ifndef SCRIPTSWITCHERCOMPONENT_H
#define SCRIPTSWITCHERCOMPONENT_H

#include <imgui.h>
#include "Component.h"
#include "Components/ScriptComponent.h"
#include "IImGuiRenderable.h"
#include <vector>
#include <string>

namespace spark
{
    class ScriptSwitcherComponent final : public Component, public IInspectorRenderable
    {
    public:
        ScriptSwitcherComponent(GameObject *parent, const std::vector<std::string> &scripts, GameObject *scriptRunner)
            : Component(parent), m_scripts{scripts}, m_scriptRunner{scriptRunner}, m_currentScriptIndex{-1}
        {
            // Find the current script index if the script runner already has a script loaded
            if (m_scriptRunner)
            {
                auto scriptComp = m_scriptRunner->GetComponent<ScriptComponent>();
                if (scriptComp)
                {
                    std::string currentPath = scriptComp->GetScriptPath();
                    for (size_t i = 0; i < m_scripts.size(); ++i)
                    {
                        if (m_scripts[i] == currentPath)
                        {
                            m_currentScriptIndex = static_cast<int>(i);
                            break;
                        }
                    }
                }
            }
        }

        void RenderInspector() override
        {
            if (ImGui::CollapsingHeader("Script Switcher", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (!m_scriptRunner)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: No script runner assigned!");
                    return;
                }

                auto scriptComp = m_scriptRunner->GetComponent<ScriptComponent>();
                if (!scriptComp)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: Script runner has no ScriptComponent!");
                    return;
                }

                // Display current script
                std::string currentScript = scriptComp->GetScriptPath();
                if (currentScript.empty())
                {
                    ImGui::Text("Current Script: (None)");
                }
                else
                {
                    ImGui::Text("Current Script: %s", ExtractFileName(currentScript).c_str());
                }

                ImGui::Separator();

                // Display available scripts as buttons
                ImGui::Text("Available Scripts:");
                for (size_t i = 0; i < m_scripts.size(); ++i)
                {
                    std::string fileName = ExtractFileName(m_scripts[i]);
                    
                    // Highlight current script button
                    bool isCurrentScript = (static_cast<int>(i) == m_currentScriptIndex);
                    if (isCurrentScript)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.7f, 0.0f, 0.6f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.8f, 0.0f, 0.8f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.9f, 0.0f, 1.0f));
                    }

                    if (ImGui::Button(fileName.c_str(), ImVec2(200, 30)))
                    {
                        SwitchToScript(static_cast<int>(i));
                    }

                    if (isCurrentScript)
                    {
                        ImGui::PopStyleColor(3);
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(Current)");
                    }
                }

                ImGui::Separator();

                // Additional controls
                if (ImGui::Button("Reload Current Script", ImVec2(200, 30)))
                {
                    ReloadCurrentScript();
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Clear Script", ImVec2(120, 30)))
                {
                    ClearScript();
                }
            }
        }

        // Public methods for programmatic control
        void SwitchToScript(int index)
        {
            if (index < 0 || index >= static_cast<int>(m_scripts.size()))
            {
                std::cerr << "ScriptSwitcherComponent: Invalid script index " << index << std::endl;
                return;
            }

            if (!m_scriptRunner)
            {
                std::cerr << "ScriptSwitcherComponent: No script runner assigned!" << std::endl;
                return;
            }

            auto scriptComp = m_scriptRunner->GetComponent<ScriptComponent>();
            if (!scriptComp)
            {
                std::cerr << "ScriptSwitcherComponent: Script runner has no ScriptComponent!" << std::endl;
                return;
            }

            // Switch to the new script
            std::string newScriptPath = m_scripts[index];
            scriptComp->SetScriptPath(newScriptPath);
            
            if (scriptComp->LoadScriptContent() && scriptComp->ReloadScript())
            {
                m_currentScriptIndex = index;
                std::cout << "Successfully switched to script: " << ExtractFileName(newScriptPath) << std::endl;
            }
            else
            {
                std::cerr << "Failed to switch to script: " << newScriptPath << std::endl;
            }
        }

        void ReloadCurrentScript()
        {
            if (!m_scriptRunner)
            {
                std::cerr << "ScriptSwitcherComponent: No script runner assigned!" << std::endl;
                return;
            }

            auto scriptComp = m_scriptRunner->GetComponent<ScriptComponent>();
            if (!scriptComp)
            {
                std::cerr << "ScriptSwitcherComponent: Script runner has no ScriptComponent!" << std::endl;
                return;
            }

            if (scriptComp->ReloadScript())
            {
                std::cout << "Successfully reloaded current script." << std::endl;
            }
            else
            {
                std::cerr << "Failed to reload current script." << std::endl;
            }
        }

        void ClearScript()
        {
            if (!m_scriptRunner)
            {
                std::cerr << "ScriptSwitcherComponent: No script runner assigned!" << std::endl;
                return;
            }

            auto scriptComp = m_scriptRunner->GetComponent<ScriptComponent>();
            if (!scriptComp)
            {
                std::cerr << "ScriptSwitcherComponent: Script runner has no ScriptComponent!" << std::endl;
                return;
            }

            scriptComp->SetScriptPath("");
            scriptComp->ClearScriptContent();
            m_currentScriptIndex = -1;
            std::cout << "Script cleared." << std::endl;
        }

        // Getters and setters
        void SetScripts(const std::vector<std::string> &scripts) { m_scripts = scripts; }
        const std::vector<std::string> &GetScripts() const { return m_scripts; }
        void SetScriptRunner(GameObject *scriptRunner) { m_scriptRunner = scriptRunner; }
        GameObject *GetScriptRunner() const { return m_scriptRunner; }
        int GetCurrentScriptIndex() const { return m_currentScriptIndex; }

    private:
        std::vector<std::string> m_scripts;
        GameObject *m_scriptRunner;
        int m_currentScriptIndex; // -1 means no script or unknown script

        // Helper function to extract filename from path
        std::string ExtractFileName(const std::string &path) const
        {
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos)
            {
                return path.substr(lastSlash + 1);
            }
            return path;
        }
    };
}

#endif
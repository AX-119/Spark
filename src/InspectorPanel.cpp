#include "InspectorPanel.h"
#include "SceneManager.h"
#include "GameObject.h"
#include <imgui.h>
namespace spark
{
    void InspectorPanel::Render(SceneManager &sceneManager, GameObject *selectedGameObject)
    {
        ImGui::Begin("Inspector Panel", nullptr, ImGuiWindowFlags_NoCollapse);
        if (selectedGameObject)
        {
            ImGui::Text("Inspector:");
            ImGui::Separator();
            selectedGameObject->RenderInspector(); // Use the new method
        }
        else
        {
            ImGui::Text("No GameObject selected");
        }

        ImGui::End();
    }
}
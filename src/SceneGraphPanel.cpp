#include "SceneGraphPanel.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "imgui.h"

namespace spark
{
    void SceneGraphPanel::Render(SceneManager &sceneManager, GameObject *&selectedGameObject) // ew
    {
        ImGui::Begin("SceneGraph", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Hierarchy:");
        ImGui::Separator();

        // Add all gameobjects and their children to the scenegraph
        const auto &allGameObjects = sceneManager.GetCurrentScene()->GetAllGameObjects();
        for (const auto &gameObject : allGameObjects)
        {
            if (!gameObject->GetParent())
            {
                RenderGameObjectNode(gameObject, selectedGameObject);
            }
        }
        ImGui::End();
    }

    void SceneGraphPanel::RenderGameObjectNode(GameObject *obj, GameObject *&selectedGameObject)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        const auto &children = obj->GetChildren();
        if (children.empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }
        if (obj == selectedGameObject)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        bool nodeOpen = ImGui::TreeNodeEx((void *)obj, flags, "%s", obj->GetName().c_str());
        if (nodeOpen)
        {
            if (ImGui::IsItemClicked())
            {
                selectedGameObject = obj;
            }

            for (auto child : children)
            {
                RenderGameObjectNode(child, selectedGameObject);
            }
            ImGui::TreePop();
        }
    }

}
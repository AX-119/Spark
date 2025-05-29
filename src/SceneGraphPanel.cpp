#include "SceneGraphPanel.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "imgui.h"

namespace spark
{
    void SceneGraphPanel::Render(SceneManager &sceneManager, GameObject *selectedGameObject)
    {
        ImGui::Begin("SceneGraph", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Hierarchy:");
        ImGui::Separator();

        // render all gameobject and their children
        const auto &allGameObjects = sceneManager.GetCurrentScene()->GetAllGameObjects();
        for (const auto &gameObject : allGameObjects)
        {
            if (!gameObject->GetParent())
            {
                RenderGameObjectNode(gameObject);
            }
        }
        ImGui::End();
    }

    void SceneGraphPanel::RenderGameObjectNode(GameObject *obj)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        const auto &children = obj->GetChildren();
        if (children.empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void *)obj, flags, "%s", obj->GetName().c_str());

        if (nodeOpen)
        {
            for (auto child : children)
            {
                RenderGameObjectNode(child);
            }
            ImGui::TreePop();
        }
    }

}
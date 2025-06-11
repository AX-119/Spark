#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H

#include <imgui.h>
#include "Component.h"
#include "Components/TransformComponent.h"
#include "IImGuiRenderable.h"
#include <SDL3/SDL.h>

namespace spark
{
    class TestComponent final : public Component, public IImGuiRenderable, public IInspectorRenderable
    {
    public:
        TestComponent(GameObject *parent) : Component(parent) {}
        void RenderImGui() override
        {
            // This creates a separate debug window (not in inspector)
            ImGui::Begin("TestComponent Debug Window");
            ImGui::SetWindowPos({GetParent()->GetComponent<TransformComponent>()->GetWorldPosition().x, GetParent()->GetComponent<TransformComponent>()->GetWorldPosition().y});
            ImGui::Text("This is a debug window, created by the test component");
            ImGui::End();
        }
        void RenderInspector() override
        {
            // This renders in the inspector panel when the GameObject is selected
            if (ImGui::CollapsingHeader("Test Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("This component demonstrates inspector rendering");

                if (ImGui::Button("Add 100 to Position", {200, 30}))
                {
                    auto currentPos = GetParent()->GetComponent<TransformComponent>()->GetLocalPosition();
                    auto newPos = currentPos + glm::vec3{100, 100, 100};
                    GetParent()->GetComponent<TransformComponent>()->SetLocalPosition(newPos);
                }

                ImGui::Separator();
            }
        }
    };
}

#endif // TESTCOMPONENT_H

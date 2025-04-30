#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H

#include <imgui.h>
#include "Component.h"
#include "Components/TransformComponent.h"
#include "IImGuiRenderable.h"
#include <SDL3/SDL.h>

namespace spark
{
    class TestComponent final : public Component, public IImGuiRenderable
    {
    public:
        TestComponent(GameObject *parent) : Component(parent) {}
        void RenderImGui() override
        {
            ImGui::Begin("TestComponent");
            ImGui::SetWindowPos({GetParent()->GetComponent<TransformComponent>()->GetWorldPosition().x, GetParent()->GetComponent<TransformComponent>()->GetWorldPosition().y});
            ImGui::Text("This is a window, created by the test component");
            if (ImGui::Button("add100", {200, 100}))
            {
                auto currentPos = GetParent()->GetComponent<TransformComponent>()->GetLocalPosition();
                auto newPos = currentPos + glm::vec3{100, 100, 100};
                GetParent()->GetComponent<TransformComponent>()->SetLocalPosition(newPos);
            }
            ImGui::End();
        }
    };
}

#endif // TESTCOMPONENT_H

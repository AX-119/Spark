#include "EditorUI.h"
#include "SceneManager.h"
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace spark
{
    EditorUI::EditorUI() : m_sceneGraphPanel{std::make_unique<SceneGraphPanel>()}
    {
    }

    void EditorUI::Init(SDL_Window *window, SDL_Renderer *renderer)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
    }

    void EditorUI::Shutdown()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
    void EditorUI::ProcessEvent(const SDL_Event *e)
    {
        ImGui_ImplSDL3_ProcessEvent(e);
    }
    void EditorUI::BeginFrame()
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
    void EditorUI::Render(SceneManager &sceneManager)
    {
        SetupDockspace();
        if (m_sceneGraphPanel)
        {
            m_sceneGraphPanel->Render(sceneManager, m_selectedGameObject);
        }
        // if (m_inspectorPanel && m_selectedGameObject) {
        //     m_inspectorPanel->Render(m_selectedGameObject);
        // }
        sceneManager.ImGuiRender();
    }
    void EditorUI::EndFrame(SDL_Renderer *renderer)
    {
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

    void EditorUI::SetupDockspace()
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(dockspaceID, viewport, dockspaceFlags);
    }
}
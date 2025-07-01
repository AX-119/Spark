#include "EditorUI.h"
#include "SceneManager.h"
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten_browser_clipboard.h>
#endif

namespace spark
{

    EditorUI::EditorUI() : m_sceneGraphPanel{std::make_unique<SceneGraphPanel>()}, m_inspectorPanel{std::make_unique<InspectorPanel>()}

    {
    }

    void EditorUI::Init(SDL_Window *window, SDL_Renderer *renderer)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = "res/configs/imgui.ini";
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/RedHatMono-Regular.ttf", m_fontSize);

        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);

#ifdef __EMSCRIPTEN__
        emscripten_browser_clipboard::paste([](std::string &&paste_data, void *callback_data [[maybe_unused]])
                                            {
                                                std::cout << "Browser pasted: " << paste_data << "\n";
                                                g_clipboardContent = std::move(paste_data); });

        io.GetClipboardTextFn = GetClipboardForImGui;
        io.SetClipboardTextFn = SetClipboardFromImGui;
#endif
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
        // RenderPlaybackControls();

        if (m_sceneGraphPanel)
        {
            m_sceneGraphPanel->Render(sceneManager, m_selectedGameObject);
        }
        if (m_inspectorPanel && m_selectedGameObject)
        {
            m_inspectorPanel->Render(sceneManager, m_selectedGameObject);
        }
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

    void EditorUI::RenderPlaybackControls()
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 windowSize = ImVec2(120, 60);
        ImVec2 windowPos = ImVec2(
            viewport->Pos.x + (viewport->Size.x - windowSize.x) * 0.5f,
            viewport->Pos.y + 10);

        ImGui::SetNextWindowPos(windowPos);
        ImGui::SetNextWindowSize(windowSize);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoDocking;

        if (ImGui::Begin("Playback Controls", nullptr, flags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));

            if (m_isPlaying)
            {
                if (ImGui::Button("⏸", ImVec2(30, 30)))
                {
                    m_isPlaying = false;
                }
            }
            else
            {
                if (ImGui::Button("▶", ImVec2(30, 30)))
                {
                    m_isPlaying = true;
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("⏹", ImVec2(30, 30)))
            {
                m_isPlaying = false;
            }

            ImGui::PopStyleVar();
        }
        ImGui::End();
    }

    std::string g_clipboardContent{};
    char const *GetClipboardForImGui(void *userData [[maybe_unused]])
    {
        std::cout << "GetClipboardForImGui called" << std::endl;
        return g_clipboardContent.c_str();
    }
    void SetClipboardFromImGui(void *userData [[maybe_unused]], char const *text)
    {
        std::cout << "Setting clipboard from ImGui: " << text << "\n";
        g_clipboardContent = text;
        emscripten_browser_clipboard::copy(g_clipboardContent);
    }
}
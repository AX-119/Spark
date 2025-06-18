#ifndef EDITORUI_H
#define EDITORUI_H

#include <SDL3/SDL.h>
#include <imgui.h>
#include <memory>
#include "SceneGraphPanel.h"
#include "InspectorPanel.h"

namespace spark
{
    class SceneManager;
    class GameObject;
    class EditorUI final
    {
    public:
        EditorUI();
        ~EditorUI() = default;

        void Init(SDL_Window *window, SDL_Renderer *renderer);
        void Shutdown();

        void ProcessEvent(const SDL_Event *e);
        void BeginFrame();
        void Render(SceneManager &sceneManager);
        void EndFrame(SDL_Renderer *renderer);

    private:
        void SetupDockspace();
        void RenderPlaybackControls();
        std::unique_ptr<SceneGraphPanel> m_sceneGraphPanel;
        std::unique_ptr<InspectorPanel> m_inspectorPanel;

        GameObject *m_selectedGameObject{nullptr};
        bool m_isPlaying{};
    };
}

#endif // EDITORUI_H
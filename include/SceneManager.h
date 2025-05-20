#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>

#include "Scene.h"
namespace spark
{
    class SceneManager final : public Singleton<SceneManager>
    {
    public:
        SceneManager(const SceneManager &other) = delete;
        SceneManager(SceneManager &&other) = delete;
        SceneManager &operator=(const SceneManager &other) = delete;
        SceneManager &operator=(SceneManager &&other) = delete;

        void AddScene(std::unique_ptr<Scene> scene);
        Scene *EmplaceScene(const std::string &name = "");

        Scene *GetSceneByName(const std::string &name);
        Scene *GetCurrentScene();

        void SwitchToScene(Scene *scene);
        void SwitchToScene(const std::string &sceneName);

        void RemoveScene(Scene *scene);
        void RemoveScene(const std::string &name);

        void Init();
        void Update(float dt);
        void Render();
        void ImGuiRender();

    private:
        friend Singleton<SceneManager>;
        SceneManager();
        std::vector<std::unique_ptr<Scene>> m_Scenes;
        Scene *m_currentScene;
    };
}
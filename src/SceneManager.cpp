#include "SceneManager.h"

namespace spark
{

    SceneManager::SceneManager()
    {
        auto scene = EmplaceScene("default scene");
        SwitchToScene(scene);
    }

    void SceneManager::Init()
    {
        m_currentScene->Init();
    }

    void SceneManager::Update(float dt)
    {
        m_currentScene->Update(dt);
    }

    void SceneManager::Render()
    {
        m_currentScene->Render();
    }

    void SceneManager::ImGuiRender()
    {
        m_currentScene->RenderImGui();
    }

    void SceneManager::AddScene(std::unique_ptr<Scene> scene)
    {
        auto it = std::find_if(m_Scenes.begin(), m_Scenes.end(), [&](const std::unique_ptr<Scene> &s)
                               { return s.get() == scene.get(); });
        if (it != m_Scenes.end())
        {
            // scene ptr was found in m_Scenes
            // TODO: add logging / error handling
            return;
        }
        m_Scenes.emplace_back(std::move(scene));
    }

    Scene *SceneManager::EmplaceScene(const std::string &name)
    {
        auto scene = std::make_unique<Scene>(name);
        auto scenePtr = scene.get();
        m_Scenes.emplace_back(std::move(scene));
        return scenePtr;
    }
    Scene *SceneManager::GetSceneByName(const std::string &name)
    {
        auto it = std::find_if(m_Scenes.begin(), m_Scenes.end(), [&](const std::unique_ptr<Scene> &scene)
                               { return scene->GetName() == name; });
        if (it == m_Scenes.end())
        {
            // Scene not found
            return nullptr;
        }
        return it->get();
    }
    Scene *SceneManager::GetCurrentScene()
    {
        return m_currentScene;
    }
    void SceneManager::SwitchToScene(Scene *scene)
    {
        m_currentScene = scene;
    }
    void SceneManager::SwitchToScene(const std::string &sceneName)
    {
        Scene *scene = GetSceneByName(sceneName);
        if (!scene)
        {
            // TODO: Handle error state
            return;
        }
        m_currentScene = scene;
    }
    void SceneManager::RemoveScene(Scene *scene)
    {
        if (m_currentScene != scene)
        {
            std::erase_if(m_Scenes, [&](const std::unique_ptr<Scene> &scenePtr)
                          { return scenePtr.get() == scene; });
        }
        else
        {
            // Log error message or something
        }
    }
    void SceneManager::RemoveScene(const std::string &name)
    {
        Scene *scene = GetSceneByName(name);
        if (scene)
        {
            RemoveScene(scene);
        }
        else
        {
            // log stuff
        }
    }
}

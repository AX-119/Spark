#include "Scene.h"
#include "GameObject.h"
#include <ranges>

namespace spark
{
    Scene::Scene(const std::string &name) : m_name{name}
    {
    }

    void Scene::AddGameObject(GameObject *gameObject)
    {
        bool isAlreadyPresent = std::ranges::find_if(m_gameObjects, [&gameObject](const std::unique_ptr<GameObject> &go)
                                                     { return go.get() == gameObject; }) != m_gameObjects.end();
        if (gameObject && !isAlreadyPresent)
        {
            m_gameObjects.emplace_back(std::unique_ptr<GameObject>(gameObject));
        }
    }

    GameObject *Scene::EmplaceGameObject()
    {
        auto &ref = m_gameObjects.emplace_back(std::make_unique<GameObject>());
        return ref.get();
    }

    void Scene::RemoveGameObject(GameObject *gameObject)
    {
        std::erase_if(m_gameObjects, [&](const std::unique_ptr<GameObject> &go)
                      { return go.get() == gameObject; });
    }

    void Scene::SetName(const std::string &name)
    {
        if (!name.empty())
        {

            m_name = name;
        }
        else
        {
            m_name = "Scene";
        }
    }

    std::string Scene::GetName() const
    {
        return std::string();
    }

    void Scene::DeleteGameObjects()
    {
        std::erase_if(m_gameObjects, [](const std::unique_ptr<GameObject> &go)
                      { return go->GetIsToBeDeleted(); });
    }
    void Scene::Init()
    {
        for (auto &go : m_gameObjects)
        {
            go->Init();
        }
    }
    void Scene::Update(float dt)
    {
        for (auto &go : m_gameObjects)
        {
            go->Update(dt);
        }
        DeleteGameObjects();
    }
    void Scene::Render()
    {
        for (auto &go : m_gameObjects)
        {
            go->Render();
        }
    }
    void Scene::RenderImGui()
    {
        for (auto &go : m_gameObjects)
        {
            go->RenderImGui();
        }
    }
}

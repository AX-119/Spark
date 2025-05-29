#include "Scene.h"
#include "GameObject.h"
#include <ranges>
#include <algorithm>

namespace spark
{
    Scene::Scene(const std::string &name) : m_name{name}
    {
    }

    void Scene::AddGameObject(std::unique_ptr<GameObject> gameObject)
    {
        if (!gameObject)
            return;
        GameObject *rawPtr = gameObject.get();
        bool isAlreadyPresent = std::ranges::any_of(m_gameObjects, [&](const auto &go)
                                                    { return go.get() == rawPtr; });
        if (!isAlreadyPresent)
        {
            m_gameObjects.emplace_back(std::move(gameObject));
        }
    }

    GameObject *Scene::EmplaceGameObject()
    {
        auto &ref = m_gameObjects.emplace_back(std::make_unique<GameObject>());
        return ref.get();
    }
    GameObject *Scene::EmplaceGameObject(const std::string &name)
    {
        auto &ref = m_gameObjects.emplace_back(std::make_unique<GameObject>(name));
        return ref.get();
    }

    void Scene::RemoveGameObject(GameObject *gameObject)
    {
        std::erase_if(m_gameObjects, [&](const auto &go)
                      { return go.get() == gameObject; });
    }

    GameObject *Scene::PopGameObject(GameObject *gameObject)
    {
        // Find the given GO in the list
        const auto it = std::ranges::find_if(m_gameObjects,
                                             [&](const std::unique_ptr<GameObject> &ptr)
                                             { return ptr.get() == gameObject; });

        // If found
        if (it != m_gameObjects.end())
        {
            // store the found object's raw ptr
            auto tmp = it->get();

            // Release the unique pointer from its cleanup/management duties
            //(to prevent the destructor from being called when we remove it from the owning vector)
            it->release();
            // Remove the released unique pointer from the vector
            // Which would be a unique pointer to nullptr
            m_gameObjects.erase(it);
            return tmp;
        }
        return nullptr;
    }

    std::vector<GameObject *> Scene::GetAllGameObjects() const
    {
        std::vector<GameObject *> rawPtrs;
        rawPtrs.reserve(m_gameObjects.size());
        for (const auto &go : m_gameObjects)
        {
            rawPtrs.push_back(go.get());
        }
        return rawPtrs;
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

    const std::string &Scene::GetName() const
    {
        return m_name;
    }

    void Scene::DeleteGameObjects()
    {
        std::erase_if(m_gameObjects, [](const auto &go)
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

#pragma once
#include <string>
#include <vector>
#include <memory>
#include "GameObject.h"
namespace spark
{
    class Scene final
    {
    public:
        explicit Scene(const std::string &name = "Scene");
        virtual ~Scene() = default;

        Scene(const Scene &other) = delete;
        Scene(Scene &&other) = delete;
        Scene &operator=(const Scene &other) = delete;
        Scene &operator=(Scene &&other) = delete;

        void AddGameObject(std::unique_ptr<GameObject> gameObject);
        GameObject *EmplaceGameObject();
        GameObject *EmplaceGameObject(const std::string &name);
        void RemoveGameObject(GameObject *gameObject);
        GameObject *PopGameObject(GameObject *gameObject);
        std::vector<GameObject *> GetAllGameObjects() const;

        void SetName(const std::string &name);
        const std::string &GetName() const;

        void Init();
        void Update(float dt);
        void Render();
        void RenderImGui();

    private:
        void DeleteGameObjects();

        std::string m_name{"Scene"};
        std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    };
}
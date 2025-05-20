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
        Scene() = default;
        Scene(const std::string &name);
        virtual ~Scene() = default;

        Scene(const Scene &other) = delete;
        Scene(Scene &&other) = delete;
        Scene &operator=(const Scene &other) = delete;
        Scene &operator=(Scene &&other) = delete;

        void AddGameObject(GameObject *gameObject);
        GameObject *EmplaceGameObject();
        void RemoveGameObject(GameObject *gameObject);

        void SetName(const std::string &name);
        std::string GetName() const;

        void Init();
        void Update(float dt);
        void Render();
        void RenderImGui();

    private:
        std::string m_name{"Scene"};
        std::vector<std::unique_ptr<GameObject>> m_gameObjects;

        void DeleteGameObjects();
    };
}
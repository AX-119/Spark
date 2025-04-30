#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <cstdint>
#include <vector>
#include <memory>

#include "component.h"
#include "IUpdateable.h"
#include "IRenderable.h"
#include "IImGuiRenderable.h"

namespace spark
{
    class Component;
    class TransformComponent;
    class GameObject final
    {
    public:
        explicit GameObject();

        ~GameObject() = default;

        GameObject(const GameObject &other) = delete;
        GameObject(GameObject &&other) = delete;
        GameObject &operator=(const GameObject &other) = delete;
        GameObject &operator=(GameObject &&other) = delete;

        void Update();
        void Render();
        void RenderImGui();

        void SetParent(GameObject *newParent, bool keepWorldPos = true);
        GameObject *GetParent() const;
        const std::vector<GameObject *> &GetChildren() const;
        bool IsChild(GameObject *gameObject); // check if given GO is child of this

        template <typename T, typename... Args>
        T *AddComponent(Args &&...args)
        {
            // TODO: Check if we're not adding a duplicate
            //  Might be a good idea to use dictionaries instead of vectors too
            //  could decrease the amount of dynamic casting that's happening
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
            T *rawPtr = component.get();

            components.emplace_back(std::move(component));

            if (auto *u = dynamic_cast<IUpdateable *>(rawPtr))
            {
                updateables.emplace_back(u);
            }
            if (auto *r = dynamic_cast<IRenderable *>(rawPtr))
            {
                renderables.emplace_back(r);
            }
            if (auto *i = dynamic_cast<IImGuiRenderable *>(rawPtr))
            {
                imguiRenderables.emplace_back(i);
            }
            return rawPtr;
        }

        template <typename T>
        T *GetComponent()
        {
            for (auto &c : components)
            {
                if (auto *casted = dynamic_cast<T *>(c.get()))
                {
                    return casted;
                }
            }
            return nullptr;
        }

        template <typename T>
        void RemoveComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");

            T *target = GetComponent<T>();
            if (!target)
                return;

            if (auto *u = dynamic_cast<IUpdateable *>(target))
            {
                RemoveInterfacePtr(updateables, u);
            }
            if (auto *r = dynamic_cast<IRenderable *>(target))
            {
                RemoveInterfacePtr(renderables, r);
            }
            if (auto *i = dynamic_cast<IImGuiRenderable *>(target))
            {
                RemoveInterfacePtr(imguiRenderables, i);
            }
            std::erase_if(components, [target](const std::unique_ptr<Component> &comp)
                          { return comp.get() == target; });
        }

    private:
        // uint32_t m_ID;
        // char *m_name;
        GameObject *m_parent{};
        std::vector<std::unique_ptr<GameObject>> m_children{};
        std::vector<GameObject *> m_childrenRawPtrs{};

        std::vector<std::unique_ptr<Component>> components;
        std::vector<IUpdateable *> updateables;
        std::vector<IRenderable *> renderables;
        std::vector<IImGuiRenderable *> imguiRenderables;

        TransformComponent *m_transform{};

        template <typename T>
        void RemoveInterfacePtr(std::vector<T *> &list, T *ptr)
        {
            std::erase(list, ptr);
        }
    };
}

#endif // GAMEOBJECT_H

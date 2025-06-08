#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

#include "Component.h"
#include "IInitializable.h"
#include "IUpdateable.h"
#include "IRenderable.h"
#include "IImGuiRenderable.h"
#include "IInspectorRenderable.h"

namespace spark
{
    class Component;
    class TransformComponent;
    class GameObject final
    {
    public:
        explicit GameObject();
        explicit GameObject(const std::string &name);

        ~GameObject() = default;

        GameObject(const GameObject &other) = delete;
        GameObject(GameObject &&other) = delete;
        GameObject &operator=(const GameObject &other) = delete;
        GameObject &operator=(GameObject &&other) = delete;

        void Init();
        void Update(float dt);
        void Render();
        void RenderImGui();
        void RenderInspector();

        void SetParent(GameObject *newParent, bool keepWorldPos = true);
        GameObject *GetParent() const;
        const std::vector<GameObject *> &GetChildren() const;
        bool IsChild(GameObject *gameObject); // check if given GO is child of this

        void Delete();
        bool GetIsToBeDeleted() const;

        std::string GetName() const;

        template <typename T, typename... Args>
        T *AddComponent(Args &&...args)
        {
            // TODO: Check if we're not adding a duplicate
            //  Might be a good idea to use dictionaries instead of vectors too
            //  could decrease the amount of dynamic casting that's happening
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
            T *rawPtr = component.get();

            m_components.emplace_back(std::move(component));

            if (auto *i = dynamic_cast<IInitializable *>(rawPtr))
            {
                m_initializables.emplace_back(i);
            }
            if (auto *u = dynamic_cast<IUpdateable *>(rawPtr))
            {
                m_updateables.emplace_back(u);
            }
            if (auto *r = dynamic_cast<IRenderable *>(rawPtr))
            {
                m_renderables.emplace_back(r);
            }
            if (auto *i = dynamic_cast<IImGuiRenderable *>(rawPtr))
            {
                m_imguiRenderables.emplace_back(i);
            }
            if (auto *ir = dynamic_cast<IInspectorRenderable *>(rawPtr))
            {
                m_inspectorRenderables.emplace_back(ir);
            }
            return rawPtr;
        }

        template <typename T>
        T *GetComponent()
        {
            for (auto &c : m_components)
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

            if (auto *i = dynamic_cast<IInitializable *>(target))
            {
                RemoveInterfacePtr(m_initializables, i);
            }
            if (auto *u = dynamic_cast<IUpdateable *>(target))
            {
                RemoveInterfacePtr(m_updateables, u);
            }
            if (auto *r = dynamic_cast<IRenderable *>(target))
            {
                RemoveInterfacePtr(m_renderables, r);
            }
            if (auto *i = dynamic_cast<IImGuiRenderable *>(target))
            {
                RemoveInterfacePtr(m_imguiRenderables, i);
            }
            if (auto *ir = dynamic_cast<IInspectorRenderable *>(target))
            {
                RemoveInterfacePtr(m_inspectorRenderables, ir);
            }
            std::erase_if(m_components, [target](const std::unique_ptr<Component> &comp)
                          { return comp.get() == target; });
        }

    private:
        std::string m_name{"GameObject"};
        bool m_isToBeDeleted{};
        GameObject *m_parent{};
        std::vector<std::unique_ptr<GameObject>> m_children{};
        std::vector<GameObject *> m_childrenRawPtrs{};

        std::vector<std::unique_ptr<Component>> m_components;
        std::vector<IInitializable *> m_initializables;
        std::vector<IUpdateable *> m_updateables;
        std::vector<IRenderable *> m_renderables;
        std::vector<IImGuiRenderable *> m_imguiRenderables;
        std::vector<IInspectorRenderable *> m_inspectorRenderables;

        TransformComponent *m_transform{};

        template <typename T>
        void RemoveInterfacePtr(std::vector<T *> &list, T *ptr)
        {
            std::erase(list, ptr);
        }
    };
}

#endif // GAMEOBJECT_H

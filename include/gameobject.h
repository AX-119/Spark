#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <type_traits>

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
        explicit GameObject(std::string &&name);

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
        GameObject *GetParent() const noexcept { return m_parent; }
        const std::vector<GameObject *> &GetChildren() const noexcept { return m_childrenRawPtrs; }
        bool IsChild(GameObject *gameObject) const noexcept;

        void Delete() noexcept { m_isToBeDeleted = true; }
        bool GetIsToBeDeleted() const noexcept { return m_isToBeDeleted; }

        const std::string &GetName() const noexcept { return m_name; }
        void SetName(const std::string &name) { m_name = name; }
        void SetName(std::string &&name) { m_name = std::move(name); }

        template <typename T, typename... Args>
        T *AddComponent(Args &&...args)
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");

            // Check for duplicate components
            std::type_index typeIdx = std::type_index(typeid(T));
            if (m_componentMap.find(typeIdx) != m_componentMap.end())
            {
                return static_cast<T *>(m_componentMap[typeIdx]);
            }

            auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
            T *rawPtr = component.get();

            m_componentMap[typeIdx] = rawPtr;
            m_components.emplace_back(std::move(component));

            CacheInterfacePointers(rawPtr);

            return rawPtr;
        }

        template <typename T>
        T *GetComponent() noexcept
        {
            std::type_index typeIdx = std::type_index(typeid(T));
            auto it = m_componentMap.find(typeIdx);
            return (it != m_componentMap.end()) ? static_cast<T *>(it->second) : nullptr;
        }

        template <typename T>
        const T *GetComponent() const noexcept
        {
            std::type_index typeIdx = std::type_index(typeid(T));
            auto it = m_componentMap.find(typeIdx);
            return (it != m_componentMap.end()) ? static_cast<const T *>(it->second) : nullptr;
        }

        template <typename T>
        bool HasComponent() const noexcept
        {
            std::type_index typeIdx = std::type_index(typeid(T));
            return m_componentMap.find(typeIdx) != m_componentMap.end();
        }

        template <typename T>
        void RemoveComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");

            std::type_index typeIdx = std::type_index(typeid(T));
            auto it = m_componentMap.find(typeIdx);
            if (it == m_componentMap.end())
                return;

            Component *target = it->second;

            RemoveFromInterfaceCaches(target);
            m_componentMap.erase(it);
            std::erase_if(m_components, [target](const std::unique_ptr<Component> &comp)
                          { return comp.get() == target; });
        }

        TransformComponent *GetTransform() const noexcept { return m_transform; }

    private:
        std::string m_name{"GameObject"};
        bool m_isToBeDeleted{false};
        GameObject *m_parent{nullptr};
        std::vector<std::unique_ptr<GameObject>> m_children{};
        std::vector<GameObject *> m_childrenRawPtrs{};

        std::vector<std::unique_ptr<Component>> m_components;
        std::unordered_map<std::type_index, Component *> m_componentMap;

        std::vector<IInitializable *> m_initializables;
        std::vector<IUpdateable *> m_updateables;
        std::vector<IRenderable *> m_renderables;
        std::vector<IImGuiRenderable *> m_imguiRenderables;
        std::vector<IInspectorRenderable *> m_inspectorRenderables;

        TransformComponent *m_transform{nullptr};

        void CacheInterfacePointers(Component *component);
        void RemoveFromInterfaceCaches(Component *component);

        template <typename T>
        void RemoveInterfacePtr(std::vector<T *> &list, T *ptr)
        {
            auto it = std::find(list.begin(), list.end(), ptr);
            if (it != list.end())
            {
                list.erase(it);
            }
        }
    };
}

#endif // GAMEOBJECT_H
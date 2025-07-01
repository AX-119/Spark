#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "SceneManager.h"
#include "imgui.h"
#include <algorithm>

namespace spark
{
    GameObject::GameObject()
    {
        m_transform = AddComponent<TransformComponent>();
    }

    GameObject::GameObject(const std::string &name) : m_name{name}
    {
        m_transform = AddComponent<TransformComponent>();
    }

    GameObject::GameObject(std::string &&name) : m_name{std::move(name)}
    {
        m_transform = AddComponent<TransformComponent>();
    }

    void GameObject::Init()
    {
        for (auto *initializable : m_initializables)
        {
            initializable->Init();
        }

        for (auto &child : m_children)
        {
            child->Init();
        }
    }

    void GameObject::Update(float dt)
    {
        for (auto *updateable : m_updateables)
        {
            updateable->Update(dt);
        }

        for (auto &child : m_children)
        {
            child->Update(dt);
        }
    }

    void GameObject::Render()
    {
        for (auto *renderable : m_renderables)
        {
            renderable->Render();
        }

        for (auto &child : m_children)
        {
            child->Render();
        }
    }

    void GameObject::RenderImGui()
    {
        for (auto *imguiRenderable : m_imguiRenderables)
        {
            imguiRenderable->RenderImGui();
        }

        for (auto &child : m_children)
        {
            child->RenderImGui();
        }
    }

    void GameObject::RenderInspector()
    {
        ImGui::Text("%s: ", m_name.c_str());
        ImGui::Separator();

        // Render all inspector-renderable components
        for (auto *inspectorRenderable : m_inspectorRenderables)
        {
            inspectorRenderable->RenderInspector();
        }
    }

    void GameObject::SetParent(GameObject *newParent, bool keepWorldPos)
    {
        if (newParent == m_parent || newParent == this || IsChild(newParent))
        {
            return;
        }

        if (newParent != nullptr)
        {
            if (keepWorldPos)
            {
                m_transform->SetLocalPosition(
                    m_transform->GetWorldPosition() - newParent->m_transform->GetWorldPosition());
            }
            else
            {
                m_transform->SetLocalPosition({0, 0, 0});
            }
        }
        else
        {
            m_transform->SetLocalPosition(m_transform->GetWorldPosition());
            m_transform->SetLocalRotation(m_transform->GetWorldRotation());
            m_transform->SetLocalScale(m_transform->GetWorldScale());
        }

        if (m_parent != nullptr)
        {
            auto it = std::find_if(m_parent->m_children.begin(), m_parent->m_children.end(),
                                   [this](const std::unique_ptr<GameObject> &ptr)
                                   { return ptr.get() == this; });

            if (it != m_parent->m_children.end())
            {
                it->release(); // Release ownership
                m_parent->m_children.erase(it);
            }

            auto rawIt = std::find(m_parent->m_childrenRawPtrs.begin(),
                                   m_parent->m_childrenRawPtrs.end(), this);
            if (rawIt != m_parent->m_childrenRawPtrs.end())
            {
                m_parent->m_childrenRawPtrs.erase(rawIt);
            }
        }
        else if (newParent != nullptr)
        {
            SceneManager::GetInstance().GetCurrentScene()->PopGameObject(this);
        }

        m_parent = newParent;

        if (m_parent != nullptr)
        {
            m_parent->m_children.emplace_back(this);
            m_parent->m_childrenRawPtrs.emplace_back(this);
        }
        else
        {
            SceneManager::GetInstance().GetCurrentScene()->AddGameObject(
                std::unique_ptr<GameObject>{this});
        }
    }

    bool GameObject::IsChild(GameObject *gameObject) const noexcept
    {
        return std::find(m_childrenRawPtrs.begin(), m_childrenRawPtrs.end(), gameObject) != m_childrenRawPtrs.end();
    }

    void GameObject::CacheInterfacePointers(Component *component)
    {
        if (auto *initializable = dynamic_cast<IInitializable *>(component))
        {
            m_initializables.emplace_back(initializable);
        }
        if (auto *updateable = dynamic_cast<IUpdateable *>(component))
        {
            m_updateables.emplace_back(updateable);
        }
        if (auto *renderable = dynamic_cast<IRenderable *>(component))
        {
            m_renderables.emplace_back(renderable);
        }
        if (auto *imguiRenderable = dynamic_cast<IImGuiRenderable *>(component))
        {
            m_imguiRenderables.emplace_back(imguiRenderable);
        }
        if (auto *inspectorRenderable = dynamic_cast<IInspectorRenderable *>(component))
        {
            m_inspectorRenderables.emplace_back(inspectorRenderable);
        }
    }

    void GameObject::RemoveFromInterfaceCaches(Component *component)
    {
        if (auto *initializable = dynamic_cast<IInitializable *>(component))
        {
            RemoveInterfacePtr(m_initializables, initializable);
        }
        if (auto *updateable = dynamic_cast<IUpdateable *>(component))
        {
            RemoveInterfacePtr(m_updateables, updateable);
        }
        if (auto *renderable = dynamic_cast<IRenderable *>(component))
        {
            RemoveInterfacePtr(m_renderables, renderable);
        }
        if (auto *imguiRenderable = dynamic_cast<IImGuiRenderable *>(component))
        {
            RemoveInterfacePtr(m_imguiRenderables, imguiRenderable);
        }
        if (auto *inspectorRenderable = dynamic_cast<IInspectorRenderable *>(component))
        {
            RemoveInterfacePtr(m_inspectorRenderables, inspectorRenderable);
        }
    }

} // namespace spark
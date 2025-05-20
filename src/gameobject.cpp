#include "GameObject.h"
#include "Components/TransformComponent.h"
namespace spark
{
    GameObject::GameObject()
    {
        m_transform = AddComponent<TransformComponent>();
    }
    void GameObject::Init()
    {
        for (auto &u : m_initializables)
        {
            u->Init();
        }
        for (auto &child : m_children)
        {
            child->Init();
        }
    }
    void GameObject::Update(float dt)
    {
        for (auto &u : m_updateables)
        {
            u->Update(dt);
        }
        for (auto &child : m_children)
        {
            child->Update(dt);
        }
    }
    void GameObject::Render()
    {
        for (auto &r : m_renderables)
        {
            r->Render();
        }
        for (auto &child : m_children)
        {
            child->Render();
        }
    }
    void GameObject::RenderImGui()
    {
        for (auto &i : m_imguiRenderables)
        {
            i->RenderImGui();
        }
        for (auto &child : m_children)
        {
            child->RenderImGui();
        }
    }
    void GameObject::SetParent(GameObject *newParent, bool keepWorldPos)
    {
        // Check if newParent is valid
        if (newParent == m_parent || newParent == this || IsChild(newParent))
        {
            // new parent was not valid...
            return;
        }

        if (newParent)
        {
            if (keepWorldPos)
            {
                m_transform->SetLocalPosition(m_transform->GetWorldPosition() - m_parent->m_transform->GetWorldPosition());
            }
            else
            {
                m_transform->SetLocalPosition({0, 0, 0});
            }
        }
        else
        {
            // make a child object a root object
            m_transform->SetLocalPosition(m_transform->GetWorldPosition());
            m_transform->SetLocalRotation(m_transform->GetWorldRotation());
            m_transform->SetLocalScale(m_transform->GetWorldScale());
        }
        // set our parent to newParent
        m_parent = newParent;
    }
    GameObject *GameObject::GetParent() const
    {
        return m_parent;
    }
    const std::vector<GameObject *> &GameObject::GetChildren() const
    {
        return m_childrenRawPtrs;
    }
    bool GameObject::IsChild(GameObject *gameObject)
    {
        if (auto it = std::ranges::find(m_childrenRawPtrs, gameObject); it != m_childrenRawPtrs.end())
        {
            return true;
        }
        return false;
    }
    void GameObject::Delete()
    {
        m_isToBeDeleted = true;
    }
    bool GameObject::GetIsToBeDeleted() const
    {
        return m_isToBeDeleted;
    }
} // namespace spark

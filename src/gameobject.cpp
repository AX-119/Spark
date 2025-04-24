#include "gameobject.h"
#include "Components/TransformComponent.h"
namespace spark
{
    GameObject::GameObject()
    {
        AddComponent<TransformComponent>();
    }

    void GameObject::Update()
    {
        for (auto &u : updateables)
        {
            u->Update();
        }
    }
    void GameObject::Render()
    {
        for (auto &r : renderables)
        {
            r->Render();
        }
    }
    void GameObject::RenderImGui()
    {
        for (auto &i : imguiRenderables)
        {
            i->RenderImGui();
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
            // we actually set a new parent
            if (keepWorldPos)
            {
            }
            else
            {
            }
        }
        else
        {
            // make a child object a root object
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

} // namespace spark

#include "GameObject.h"
#include "Components/TransformComponent.h"
namespace spark
{
    GameObject::GameObject()
    {
        m_transform = AddComponent<TransformComponent>();
    }

    // LUA
    // we need globals
    // "gameobject" : owning gameobject
    // "screen_width" , "screen_height"
    // gameobject.Create({50,35, 0});
    // function Update()
    //      gameobject.transform.translate(10,10,0);
    // end

    void GameObject::Update()
    {
        for (auto &u : updateables)
        {
            u->Update();
        }
        for (auto &child : m_children)
        {
            child->Update();
        }
        // for(auto& script : luaScripts)
        // {
        //     lua.script(script);
        // }
    }
    void GameObject::Render()
    {
        for (auto &r : renderables)
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
        for (auto &i : imguiRenderables)
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
            // TODO
            //  we actually set a new parent
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

} // namespace spark

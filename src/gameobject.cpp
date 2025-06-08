#include "GameObject.h"
#include "Components/TransformComponent.h"
#include <string>
#include "SceneManager.h"
#include "imgui.h"
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

    void GameObject::RenderInspector()
    {
        ImGui::Text("%s: ", m_name.c_str());
        ImGui::Separator();

        // Render all inspector-renderable components
        for (auto &ir : m_inspectorRenderables)
        {
            ir->RenderInspector();
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
                m_transform->SetLocalPosition(m_transform->GetWorldPosition() - newParent->m_transform->GetWorldPosition());
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

        if (m_parent)
        {
            const auto it = std::ranges::find_if(m_parent->m_children,
                                                 [&](const std::unique_ptr<GameObject> &ptr)
                                                 { return ptr.get() == this; });

            // If found
            if (it != m_parent->m_children.end())
            {
                // Release the unique pointer from its cleanup/management duties
                //(to prevent the destructor from being called when we remove it from the owning vector)
                it->release();
                // Remove the released unique pointer from the vector
                // Which would be a unique pointer to nullptr
                m_parent->m_children.erase(it);
            }
        }
        else if (m_parent == nullptr && newParent != nullptr)
        {
            // Remove this object from the Scene's GO list
            // Can't use the Remove function as the GO's are stored as unique pointers.
            // Remove would destroy this object
            // The Pop function releases a given object, erases the unique pointer from the scene's list and returns the raw pointer. Ready to transfer ownership
            // Maybe a more descriptive name is necessary...
            SceneManager::GetInstance().GetCurrentScene()->PopGameObject(this);
            // parent->m_children.emplace_back(obj);
        }
        // We can now overwrite who our parent is
        m_parent = newParent;

        if (m_parent)
        {
            // Using emplace_back will construct the unique pointer from the raw this pointer we pass in
            m_parent->m_children.emplace_back(this);
            m_parent->m_childrenRawPtrs.emplace_back(this);
        }
        else
        {
            SceneManager::GetInstance().GetCurrentScene()->AddGameObject(std::unique_ptr<GameObject>{this});
        }
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
    std::string GameObject::GetName() const
    {
        return m_name;
    }
} // namespace spark

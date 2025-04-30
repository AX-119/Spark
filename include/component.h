#ifndef COMPONENT_H
#define COMPONENT_H

#include "GameObject.h"
namespace spark
{
    class GameObject;
    class Component
    {
    public:
        explicit Component(GameObject *parent) : m_parent{parent} {}
        virtual ~Component() = default;
        GameObject *GetParent() const { return m_parent; }

    private:
        GameObject *m_parent{};
    };
}

#endif // COMPONENT_H

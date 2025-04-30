#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
namespace spark
{
    class GameObject;
    class TransformComponent final : public Component
    {
    public:
        explicit TransformComponent(GameObject *parent) : Component(parent) {};
        virtual ~TransformComponent() override = default;

        // --- Local Transformations ---

        void SetLocalPosition(const glm::vec3 &position);
        void SetLocalRotation(const glm::quat &rotation);
        void SetLocalScale(const glm::vec3 &scale);

        const glm::vec3 &GetLocalPosition() const;
        const glm::quat &GetLocalRotation() const;
        const glm::vec3 &GetLocalScale() const;

        // --- World Transformations ---

        const glm::mat4 &GetWorldMatrix() const;
        glm::vec3 GetWorldPosition() const;
        glm::quat GetWorldRotation() const;
        glm::vec3 GetWorldScale() const;

        // --- Utility ---

        glm::mat4 CalculateLocalMatrix() const;
        void SetDirtyRecursive();

    private:
        // Local space properties
        glm::vec3 m_localPosition = glm::vec3(0.0f);
        glm::quat m_localRotation = glm::identity<glm::quat>();
        glm::vec3 m_localScale = glm::vec3(1.0f);

        // World space cache
        mutable glm::mat4 m_worldMatrix = glm::identity<glm::mat4>();
        mutable bool m_isDirty = true;
    };
} // namespace spark

#endif // TRANSFORMCOMPONENT_H

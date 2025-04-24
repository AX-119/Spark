#include <Components/TransformComponent.h>
void spark::TransformComponent::SetLocalPosition(const glm::vec3 &position)
{
    if (m_localPosition != position)
    {
        m_localPosition = position;
        SetDirtyRecursive();
    }
}

void spark::TransformComponent::SetLocalRotation(const glm::quat &rotation)
{
    if (m_localRotation != rotation)
    {
        m_localRotation = rotation;
    }
}

void spark::TransformComponent::SetLocalScale(const glm::vec3 &scale)
{
    if (m_localScale != scale)
    {
        m_localScale = scale;
        SetDirtyRecursive();
    }
}
const glm::vec3 &spark::TransformComponent::GetLocalPosition() const
{
    return m_localPosition;
}
const glm::quat &spark::TransformComponent::GetLocalRotation() const
{
    return m_localRotation;
}
const glm::vec3 &spark::TransformComponent::GetLocalScale() const
{
    return m_localScale;
}

const glm::mat4 &spark::TransformComponent::GetWorldMatrix() const
{
    if (!m_isDirty)
    {
        return m_worldMatrix;
    }

    glm::mat4 localMatrix = CalculateLocalMatrix();

    GameObject *owner = GetParent();
    glm::mat4 parentWorldMatrix = glm::identity<glm::mat4>();
    if (owner)
    {
        GameObject *parentGameObject = owner->GetParent();
        if (parentGameObject)
        {
            TransformComponent *parentTransform = parentGameObject->GetComponent<TransformComponent>();
            if (parentTransform)
            {
                parentWorldMatrix = parentTransform->GetWorldMatrix();
            }
        }
    }
    m_worldMatrix = parentWorldMatrix * localMatrix;

    m_isDirty = false;
    return m_worldMatrix;
}

void spark::TransformComponent::SetDirtyRecursive()
{
    if (m_isDirty)
    {
        return;
    }

    m_isDirty = true;

    GameObject *owner = GetParent();
    if (owner)
    {

        const auto &children = owner->GetChildren();
        for (GameObject *childGameObject : children)
        {
            if (childGameObject)
            {
                TransformComponent *childTransform = childGameObject->GetComponent<TransformComponent>();
                if (childTransform)
                {
                    childTransform->SetDirtyRecursive();
                }
            }
        }
    }
}

glm::mat4 spark::TransformComponent::CalculateLocalMatrix() const
{

    const glm::mat4 identityMat = glm::identity<glm::mat4>();
    glm::mat4 transMat = glm::translate(identityMat, m_localPosition);
    glm::mat4 rotMat = glm::mat4_cast(m_localRotation);
    glm::mat4 scaleMat = glm::scale(identityMat, m_localScale);

    return transMat * rotMat * scaleMat;
}

glm::vec3 spark::TransformComponent::GetWorldScale() const
{
    GetWorldMatrix();
    return glm::vec3(
        glm::length(glm::vec3(m_worldMatrix[0])),
        glm::length(glm::vec3(m_worldMatrix[1])),
        glm::length(glm::vec3(m_worldMatrix[2])));
}
glm::quat spark::TransformComponent::GetWorldRotation() const
{
    GetWorldMatrix();
    glm::vec3 scale = GetWorldScale();
    glm::mat3 rotMat(
        glm::vec3(m_worldMatrix[0]) / scale.x,
        glm::vec3(m_worldMatrix[1]) / scale.y,
        glm::vec3(m_worldMatrix[2]) / scale.z);
    return glm::quat_cast(rotMat);
}

glm::vec3 spark::TransformComponent::GetWorldPosition() const
{
    GetWorldMatrix();
    return glm::vec3(m_worldMatrix[3]);
}

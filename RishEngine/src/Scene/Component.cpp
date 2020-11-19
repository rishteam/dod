#include <Rish/Scene/Component.h>
#include <Rish/Scene/Entity.h>

namespace rl {

void GroupComponent::addEntityUUID(const UUID &id)
{
    childEntity.insert(id);
}

void GroupComponent::delEntityUUID(const UUID &id)
{
    childEntity.erase(id);
}

void GroupComponent::clear()
{
    childEntity.clear();
}

void SubGroupComponent::setGroup(const UUID &id)
{
    groupEntity = id;
}

void SubGroupComponent::setRelativePosition(glm::vec3 pos)
{
    m_relativePosition = pos;
}

void SubGroupComponent::setGroupPosition(glm::vec3 pos)
{
    m_groupPosition = pos;
}

void SubGroupComponent::setOriginScale(glm::vec3 scale)
{
    m_originScale = scale;
}

glm::vec3 SubGroupComponent::getRelativePosition()
{
    return m_relativePosition;
}

glm::vec3 SubGroupComponent::getOriginScale()
{
    return m_originScale;
}

glm::vec3 SubGroupComponent::calculateCurrentPosition()
{
    return m_groupPosition+m_relativePosition*m_offset;
}

glm::vec3 SubGroupComponent::calculateCurrentScale()
{
    return m_originScale*m_offset;
}

} // end of namespace
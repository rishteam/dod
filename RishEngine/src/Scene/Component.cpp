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

glm::vec3 SubGroupComponent::calculateCurrentPosition()
{
    return m_groupPosition+m_relativePosition;
}

} // end of namespace
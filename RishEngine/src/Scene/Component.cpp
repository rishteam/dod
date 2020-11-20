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

glm::vec3 SubGroupComponent::calculateCurrentPosition()
{
    auto pos = m_relativePosition*m_offset;
    auto sinR = std::sin(glm::radians(m_groupRotate-m_preRotate));
    auto cosR = std::cos(glm::radians(m_groupRotate-m_preRotate));

    return m_groupPosition + glm::vec3(pos.x*cosR - pos.y*sinR, pos.x*sinR + pos.y*cosR , 0.f);

}

glm::vec3 SubGroupComponent::calculateCurrentScale()
{
    return m_originScale*m_offset;
}

float  SubGroupComponent::calculateCurrentRotate()
{
    return m_originRotate+m_groupRotate;
}

} // end of namespace
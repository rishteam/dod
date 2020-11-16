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

} // end of namespace
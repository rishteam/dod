#include "Rish/Scene/Entity.h"

namespace rl {

Entity::Entity(entt::entity handle, Scene *scene)
	: m_entityHandle(handle),
	  m_scene(scene)
	{
		RL_CORE_TRACE("[Entity] Entity create");
	}

bool Entity::operator==(const Entity &rhs) const
{
    return m_entityHandle == rhs.m_entityHandle;
}

bool Entity::operator!=(const Entity &rhs) const
{
    return !(rhs == *this);
}

}
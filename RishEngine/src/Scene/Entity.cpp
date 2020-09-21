#include "Rish/Scene/Entity.h"

namespace rl {

Entity::Entity(entt::entity handle, Scene *scene)
	: m_entityHandle(handle),
	  m_scene(scene)
{
}

}
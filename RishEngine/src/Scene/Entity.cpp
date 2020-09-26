#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

namespace rl {

Entity::Entity(entt::entity handle, Scene *scene)
	: m_entityHandle(handle),
	  m_scene(scene)
{
}

}
#pragma once

#include "Rish/rlpch.h"
#include "Rish/Scene/Scene.h"

#include "entt/entt.hpp"

namespace rl {

class Entity
{
public:

	Entity() = default;
	Entity(entt::entity handle, Scene *scene);
	Entity(const Entity &other) = default;

	template<typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		RL_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
		return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent()
	{
		RL_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
		return m_scene->m_registry.get<T>(m_entityHandle);
	}

	template<typename T>
	void removeComponent()
	{
		RL_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
		m_scene->m_registry.remove<T>(m_entityHandle);
	}

	template<typename T>
	bool hasComponent()
	{
		return m_scene->m_registry.has<T>(m_entityHandle);
	}

	void destroy()
	{
		m_scene->m_registry.destroy(m_entityHandle);
		m_scene = nullptr;
	}

	operator bool() const { return (uint32_t)m_entityHandle != 0; }

    bool operator==(const Entity &rhs) const;

    bool operator!=(const Entity &rhs) const;

private:

	entt::entity m_entityHandle{entt::null};
	Scene *m_scene = nullptr;
};

}

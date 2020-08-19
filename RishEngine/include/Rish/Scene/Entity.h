#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Component.h>

#include <entt/entt.hpp>

namespace rl {

/**
 * @brief entity object
 * @details Entity can add component or remove component through rl::Scene.
 */
class Entity
{
public:
	Entity() = default;
	Entity(const Entity &other) = default;
	Entity(entt::entity handle, Scene *scene);

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

	operator bool() const { return (uint32_t)m_entityHandle != entt::null; }

    bool operator==(const Entity &rhs) const;
    bool operator!=(const Entity &rhs) const;

private:
	entt::entity m_entityHandle{entt::null};
	Scene *m_scene = nullptr;

};

/**
 * @class rl::Entity
 * How to create an Entity and add/remove component? Use rl::Scene createEntity() function to create rl::Entity.<br/>
 * And then you can manipulate your rl::Entity.
 * 
 * @code{.cpp}
 * 
 * std::shared_ptr<rl::Scene> scene = std::make_shared<rl::Scene>();
 * 
 * // Create rl::Entity
 * auto entity = scene->createEntity();
 * 
 * // Add component
 * if(!entity.hasComponent<YOUR_COMPONENT>())
 *     entity.addComponent<YOUR_COMPONENT>();
 * 
 * // Remove component
 * if(entity.hasCompoenent<YOUR_COMPONENT>())
 *     entity.removeComponent<YOUR_COMPONENT>();
 * 
 * @endcode
 */


}

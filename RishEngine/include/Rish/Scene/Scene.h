#pragma once

#include "Rish/rlpch.h"
#include "Rish/Core/Time.h"

#include "entt/entt.hpp"

namespace rl
{

class Entity;

/**
 * @brief Scene object
 * @details A Scene is used to create and destroy entity, also update system
 */

class Scene
{

public:
	Scene();
	~Scene();

	/**
	 * @brief Create a Entity object
	 * 
	 * @return Entity 
	 */
	Entity createEntity(const std::string& name = std::string());
	void destroyEntity(const Entity& entity);

	/**
	 * @brief Update Entity behavior
	 * 
	 * @param dt 
	 */

	void update(Time dt);

private:

	static int entityNumber;

	entt::registry m_registry;

	friend class Entity;
};

} // namespace rl
#pragma once

#include "Rish/rlpch.h"
#include "Rish/Core/Time.h"

#include "entt/entt.hpp"

namespace rl
{

class Entity;

class Scene
{

public:
	Scene();
	~Scene();

	Entity createEntity(const std::string& name = std::string());
	void destroyEntity(const Entity& entity);

	void update(Time dt);

private:

	static int entityNumber;

	entt::registry m_registry;

	friend class Entity;
};

} // namespace rl
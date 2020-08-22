#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>

#include <Rish/Renderer/Camera/OrthographicCamera.h>
#include <Rish/Scene/Component.h>

#include <entt/entt.hpp>
#include <cereal/cereal.hpp>

namespace rl
{

// Forward declaration
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
	 * @param dt
	 */
	void update(const OrthographicCamera &camera, Time dt);

	// Test
	std::vector<Entity> getAllEntities();

private:

	static int entityNumber;

	entt::registry m_registry;

	friend class Entity;
	friend class cereal::access;

	// TODO: Auto reflect the components of a entity?
	template <class Archive>
	void save(Archive &ar) const
	{
		ar(CEREAL_NVP(entityNumber));

		 entt::snapshot{m_registry}
		 	.component<TagComponent, TransformComponent, RenderComponent>(ar);
	}

	template <class Archive>
	void load(Archive &ar)
	{
		RL_CORE_TRACE("Scene load");
		ar(CEREAL_NVP(entityNumber));

		entt::snapshot_loader{m_registry}
			.component<TagComponent, TransformComponent, RenderComponent>(ar);
	}
};

} // namespace rl
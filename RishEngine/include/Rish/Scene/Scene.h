#pragma once

#include "Rish/rlpch.h"
#include "Rish/Core/Time.h"

#include "entt/entt.hpp"
#include "cereal/cereal.hpp"

// for test
#include "Rish/Scene/Component.h"

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

	// Test
	std::vector<Entity> getAllEntites();

private:

	static int entityNumber;

	entt::registry m_registry;


	friend class Entity;
	friend class cereal::access;

	template <class Archive>
	void save(Archive &ar) const
	{
		ar(CEREAL_NVP(entityNumber));
		entt::snapshot{m_registry}
			.component<TagComponent, TransformComponent, RenderComponent>(ar);

		// ar(CEREAL_NVP(entityNumber));
		// m_registry.each([&](auto &entity){

			// ar(cereal::make_nvp("Entity", entity));
			// if(m_registry.has<TagComponent>(entity))
            // {
			//     ar(cereal::make_nvp("TagComponent", m_registry.get<TagComponent>(entity)));
            // }
            // if(m_registry.has<TransformComponent>(entity))
            // {
            //     ar(cereal::make_nvp("TransformComponent", m_registry.get<TransformComponent>(entity)));
            // }
            // if(m_registry.has<RenderComponent>(entity))
            // {
            //     ar(cereal::make_nvp("RenderComponent", m_registry.get<RenderComponent>(entity)));
            // }

//			m_registry.visit(entity, [&](const auto &component){
//
//				RL_CORE_TRACE("Component type {}", component);
//				ar(CEREAL_NVP(component));
//			});
		// });
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
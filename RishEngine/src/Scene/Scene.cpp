#include "Rish/Scene/Scene.h"
#include "Rish/Scene/Entity.h"
#include "Rish/Scene/Component.h"


namespace rl{

int Scene::entityNumber = 0;

Scene::Scene()
{
	entt::registry test;
	test.create();
	RL_CORE_TRACE("test");
}

Scene::~Scene()
{
}

Entity Scene::createEntity(const std::string& name)
{
	Entity entity = { m_registry.create(), this };
	entity.addComponent<TransformComponent>();
	auto& tag = entity.addComponent<TagComponent>();
	
	if(name.empty())
	{
		char num[20];
		itoa(entityNumber, num, 10);
		tag.tag = "Entity " + (std::string)num;
	}
	else
	{
		tag.tag = name;
	}
	entityNumber++;

	RL_CORE_TRACE("[Scene] In");
	return entity;	
}

void Scene::update(Time dt)
{
	// System upadte
	

	// auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	// for(auto entity: group)
	// {
	// 	// auto &[transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

	// 	auto& transform = group.get<TransformComponent>(entity);
	// 	auto& sprite	= group.get<SpriteRendererComponent>(entity);

	// }

	auto group2 = m_registry.group<TestQuadComponent, TransformComponent>(entt::get<SpriteRendererComponent>);
	for(auto entity: group2)
	{
		auto &test = group2.get<TestQuadComponent>(entity);
		auto &sprite = group2.get<SpriteRendererComponent>(entity);
		auto &transform = group2.get<TransformComponent>(entity);

		test.m_shader->bind();
		test.m_shader->setFloat4("ourColor", sprite.color);
		transform.transform = glm::translate(glm::mat4(1.0f), transform.translate);

		RL_CORE_TRACE("translate : {} {}", transform.translate.x, transform.translate.y);

		test.m_shader->setMat4("transform", transform.transform);
		test.m_vertexArray->bind();
		glDrawElements(GL_TRIANGLES, test.m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
		test.m_vertexArray->unbind();
	}
}

} // namespace rl

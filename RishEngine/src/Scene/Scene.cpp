#include <Rish/rlpch.h>

#include <Rish/Renderer/Renderer2D.h>

#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Component.h>

#include <Rish/Utils/uuid.h>

namespace rl{

int Scene::entityNumber = 0;

Scene::Scene()
{
    RL_CORE_INFO("Construct Scene");
}

Scene::~Scene()
{
    RL_CORE_INFO("Destruct Scene");
}

Entity Scene::createEntity(const std::string& name)
{
	Entity entity = { m_registry.create(), this };
	entity.addComponent<TransformComponent>();
	auto &tagComponent = entity.addComponent<TagComponent>();
	tagComponent.id = uuid::generate_uuid_v4();

    auto &tag = tagComponent.tag;
	if(name.empty())
	{
		tag = fmt::format("Entity {}", entityNumber);
	}
	else
	{
		tag = name;
	}
	entityNumber++;

	RL_CORE_TRACE("[Scene] Created entity {}", tag);
	return entity;	
}

void Scene::destroyEntity(const Entity &entity)
{
    m_registry.destroy(entity.getEntityID());
}

void Scene::onUpdate(const OrthographicCamera &camera, Time dt)
{
//    //check the trigger of collide
//    auto group = m_registry.group<TransformComponent, BoxCollider2D>();
//    for(auto entity : group)
//    {
//
//    }
//
//    //Joint
//    auto group2 = m_registry.group<TransformComponent, Joint>();
//    for(auto entity : group)
//    {
//
//    }
//
//
//    //RigidBody2D Component
//    auto group3 = m_registry.group<TransformComponent, RigidBody2D>();
//    for(auto entity : group)
//    {
//
//    }


	auto transGroup = m_registry.group<TransformComponent, RenderComponent>();
	for(auto entity: transGroup)
	{
		auto &transform = transGroup.get<TransformComponent>(entity);
		auto &render = transGroup.get<RenderComponent>(entity);

		if(render.init)
		{
			render.m_texture = Texture2D::LoadTextureVFS(render.texturePath);
			render.m_shader = Shader::LoadShaderVFS(render.vertPath, render.fragPath);
			render.init = false;
		}

		if(render.m_texture)
		    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color);
		else
		    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);

		// Draw Border
		glm::vec3 p[4];
		glm::vec2 off[] = { {-1, 1}, { 1, 1}, { 1,-1}, {-1,-1} };
		for(int i = 0; i < 4; i++) // gen vertices
            p[i] = transform.translate + transform.scale * glm::vec3(off[i], 1.f) * 0.5f;
		for(int i = 0; i < 4; i++)
            Renderer2D::DrawLine(p[i], p[(i+1)%4]);
	}
}

} // namespace rl

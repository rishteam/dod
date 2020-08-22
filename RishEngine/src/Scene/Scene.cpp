#pragma once

#include <Rish/rlpch.h>

#include <Rish/Renderer/Renderer2D.h>

#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Component.h>

namespace rl{

int Scene::entityNumber = 0;

Scene::Scene()
{
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
		tag.tag = fmt::format("Entity {}", entityNumber);
	}
	else
	{
		tag.tag = name;
	}
	entityNumber++;

	RL_CORE_TRACE("[Scene] In");
	return entity;	
}

std::vector<Entity> Scene::getAllEntities()
{
	std::vector<Entity> tmp;
	m_registry.each([&](auto entity)
	{
		tmp.push_back({entity, this});
	});
	return tmp;
}

void Scene::update(const OrthographicCamera &camera, Time dt)
{
	auto transGroup = m_registry.group<TransformComponent, RenderComponent>();
	for(auto entity: transGroup)
	{
		auto &transform = transGroup.get<TransformComponent>(entity);
		auto &render = transGroup.get<RenderComponent>(entity);

		if(render.init)
		{
			render.init = false;
		}

		if(render.reloadShader)
		{
			render.m_shader = Shader::LoadShaderVFS(render.vertPath, render.fragPath);
			render.reloadShader = false;
		}

		if(render.reloadTexture)
		{
			render.m_texture = Texture2D::LoadTextureVFS(render.texturePath);
			render.reloadTexture = false;
		}

        Renderer2D::BeginScene(camera);
		if(render.m_texture)
		    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color);
		else
		    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);

        Renderer2D::EndScene();
	}
}

} // namespace rl

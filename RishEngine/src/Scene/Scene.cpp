#pragma once

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

std::vector<Entity> Scene::getAllEntites()
{
	std::vector<Entity> tmp;
	m_registry.each([&](auto entity){

		tmp.push_back({entity, this});
	});
	return tmp;
}

void Scene::update(Time dt)
{
	// System upadte
	

	// auto group = m_registry.view<TextureComponent>();
	// for(auto entity: group)
	// {
	// 	// auto &[transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

	// 	auto &texture = group.get<TextureComponent>(entity);

	// 	if(texture.reload)
	// 	{
	// 		texture.m_texture = std::make_shared<rl::Texture2D>(texture.path);
	// 		texture.reload = false;
	// 	}
	// }


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto group2 = m_registry.group<TransformComponent>(entt::get<RenderComponent>);
	for(auto entity: group2)
	{
		// auto &test = group2.get<TestQuadComponent>(entity);
		// auto &sprite = group2.get<SpriteRendererComponent>(entity);
		auto &transform = group2.get<TransformComponent>(entity);
		auto &render = group2.get<RenderComponent>(entity);

		if(render.init)
		{
			render.m_vertexArray = std::make_shared<VertexArray>();
			std::shared_ptr<rl::VertexBuffer> vertexBuffer;
			vertexBuffer = std::make_shared<rl::VertexBuffer>(render.vertices, sizeof(render.vertices));
			rl::BufferLayout layout = {
				{rl::ShaderDataType::Float3, "a_Position"},
				{rl::ShaderDataType::Float2, "a_TexCoord"}};

			vertexBuffer->setLayout(layout);
			render.m_vertexArray->setVertexBuffer(vertexBuffer);
			std::shared_ptr<rl::IndexBuffer> indexBuffer;
			indexBuffer = std::make_shared<rl::IndexBuffer>(render.indices, sizeof(render.indices) / sizeof(uint32_t));
			render.m_vertexArray->setIndexBuffer(indexBuffer);

			render.m_vertexArray->unbind(); // Always remember to UNBIND if AMD

			render.init = false;
		}

		if(render.reload)
		{
			VFS::Get()->ResolvePhysicalPath(render.vertPath, render.vertPath);
			VFS::Get()->ResolvePhysicalPath(render.fragPath, render.fragPath);
			render.m_shader = std::make_shared<rl::Shader>(render.vertPath.c_str(), render.fragPath.c_str());
			RL_CORE_TRACE("Reload Shader");
			render.reload = false;
		}

		if(render.reloadTexture)
		{
			VFS::Get()->ResolvePhysicalPath(render.path, render.path);
			render.m_texture = std::make_shared<rl::Texture2D>(render.path);
			render.reloadTexture = false;
		}

		render.m_texture->bind();

		render.m_shader->bind();
		render.m_shader->setFloat4("ourColor", render.color);
		transform.transform = glm::translate(glm::mat4(1.0f), transform.translate);
		transform.transform = glm::scale(transform.transform, transform.scale);
		// RL_CORE_TRACE("translate : {} {}", transform.translate.x, transform.translate.y);

		render.m_shader->setMat4("transform", transform.transform);
		render.m_vertexArray->bind();
		glDrawElements(GL_TRIANGLES, render.m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
		render.m_vertexArray->unbind();

		render.m_texture->unbind();
	}
}

} // namespace rl

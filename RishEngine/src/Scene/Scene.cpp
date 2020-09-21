#include <Rish/rlpch.h>

#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/Framebuffer.h>
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Component.h>

#include <Rish/Utils/uuid.h>

#include <Rish/ImGui.h>

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

void Scene::onUpdate(Time dt)
{
    bool isAnyCamera{false};
    auto group = m_registry.view<TransformComponent, CameraComponent>();
    for(auto entity : group)
    {
        Entity ent{entity, this};
        auto &transform = ent.getComponent<TransformComponent>();
        auto &camera = ent.getComponent<CameraComponent>();

        if(camera.primary)
        {
            m_mainCamera = camera.camera;
            m_mainCameraTransform = glm::translate(glm::mat4(1.f), transform.translate);
            isAnyCamera = true;
        }
    }

    // TODO: implement multiple camera
    if(!isAnyCamera) return;

    auto cameraGroup = m_registry.group<TransformComponent, RenderComponent>();
    Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform);
    for(auto entity : cameraGroup)
    {
        Entity ent{entity, this};
        auto &transform = ent.getComponent<TransformComponent>();
        auto &render = ent.getComponent<RenderComponent>();

        if(render.m_texture)
            Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color);
        else
            Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
    }
    Renderer2D::EndScene();
}

void Scene::onImGuiRender()
{
    ImGui::Begin("SecenDebug");
    ImGui::Text("Projection");
    auto proj = m_mainCamera.getProjection();
    for(int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        for(int j = 0; j < 4; j++)
        {
            ImGui::PushID(j);
            ImGui::Text("%.2f", static_cast<double>(proj[i][j])); ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
        ImGui::PopID();
    }
    ImGui::Separator();
    ImGui::Text("Camera Transform");
    for(int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        for(int j = 0; j < 4; j++)
        {
            ImGui::PushID(j);
            ImGui::Text("%.2f", static_cast<double>(m_mainCameraTransform[i][j])); ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
        ImGui::PopID();
    }
    ImGui::End();
}

} // namespace rl

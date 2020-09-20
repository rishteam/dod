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

void Scene::onUpdate(const Ref<Framebuffer> &framebuffer, Time dt)
{
//    Camera mainCamera;
//    glm::mat4 mainCameraTransform;
    bool isAnyCamera = false;
    //
    auto group = m_registry.view<TransformComponent, CameraComponent>();
    for(auto entity : group)
    {
        Entity ent{entity, this};
        auto &transform = ent.getComponent<TransformComponent>();
        auto &camera = ent.getComponent<CameraComponent>();

        isAnyCamera = true;
        mainCamera = camera.camera;
        mainCameraTransform = glm::translate(glm::mat4(1.f), transform.translate);
    }

    if(isAnyCamera)
    {
        auto cameraGroup = m_registry.group<TransformComponent, RenderComponent>();
        Renderer2D::BeginScene(mainCamera, mainCameraTransform, framebuffer);

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
}

void Scene::onImGuiRender()
{
    ImGui::Begin("SecenDebug");
    ImGui::Text("Main Scene Camera");
    auto proj = mainCamera.getProjection();
    for(int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        ImGui::DragFloat4("", glm::value_ptr(proj[i]));
        ImGui::PopID();
    }
    ImGui::Separator();
    for(int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        ImGui::DragFloat4("", glm::value_ptr(mainCameraTransform[i]));
        ImGui::PopID();
    }
    ImGui::End();
}

} // namespace rl

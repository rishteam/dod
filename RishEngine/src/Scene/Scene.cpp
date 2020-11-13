#include <Rish/rlpch.h>
//
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/Framebuffer.h>
#include <Rish/Renderer/RendererCommand.h>
//
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>
#include <Rish/Scene/Utils.h>
// Systems
#include <Rish/Scene/System/SpriteRenderSystem.h>
#include <Rish/Effect/Particle/ParticleSystem.h>
#include <Rish/Collider/ColliderSystem.h>
#include <Rish/Physics/PhysicsSystem.h>
#include <Rish/Scene/System/NativeScriptSystem.h>
#include <Rish/Effect/Light/LightSystem.h>
//
#include <Rish/Debug/DebugWindow.h>
#include <Rish/Utils/uuid.h>
//
#include <Rish/ImGui/ImGui.h>

namespace rl{

int Scene::entityNumber = 0;

Scene::Scene()
{
    m_registry.on_construct<ParticleComponent>().connect<entt::invoke<&ParticleComponent::init>>();

    RL_CORE_INFO("Construct Scene");
}

Scene::~Scene()
{
}

Entity Scene::createEntity(const std::string& name, const glm::vec3 &pos)
{
	Entity entity = { m_registry.create(), this };
	entity.addComponent<TransformComponent>(pos);
	auto &tagComponent = entity.addComponent<TagComponent>();
	tagComponent.id = UUID();

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

    m_entNameToNumMap[tag]++;

	RL_CORE_TRACE("[Scene] Created entity {}", tag);
	return entity;
}

Entity Scene::createEntity(const UUID &id, const std::string &name)
{
    Entity entity = {m_registry.create(), this};
    entity.addComponent<TransformComponent>();
    auto &tagComponent = entity.addComponent<TagComponent>();
    tagComponent.id = id;

    auto &tag = tagComponent.tag;
    if(name.empty())
        tag = fmt::format("Entity {}", entityNumber);
    else
        tag = name;
    entityNumber++;

    m_entNameToNumMap[tag]++;

    RL_CORE_TRACE("[Scene] Created entity {} by id {}", tag, id.to_string());
    return entity;
}

void Scene::destroyEntity(Entity entity)
{
    NativeScriptSystem::OnDestroy(entity);

    m_registry.destroy(entity.getEntityID());
}

Entity Scene::duplicateEntity(Entity src)
{
    auto &tag = src.getComponent<TagComponent>().tag;
    m_entNameToNumMap[tag]++;
    auto ent = createEntity(fmt::format("{} ({})", tag, m_entNameToNumMap[tag]));

    CopyComponentToEntityIfExists<TransformComponent>(ent, src);
    CopyComponentToEntityIfExists<SpriteRenderComponent>(ent, src);
    CopyComponentToEntityIfExists<CameraComponent>(ent, src);
    CopyComponentToEntityIfExists<NativeScriptComponent>(ent, src);
    CopyComponentToEntityIfExists<RigidBody2DComponent>(ent, src);
    CopyComponentToEntityIfExists<BoxCollider2DComponent>(ent, src);
    CopyComponentToEntityIfExists<Joint2DComponent>(ent, src);
    CopyComponentToEntityIfExists<ParticleComponent>(ent, src);

    return ent;
}

void Scene::onRuntimeInit()
{
    NativeScriptSystem::OnInit();
    SpriteRenderSystem::OnInit();
}

void Scene::onEditorInit()
{
    NativeScriptSystem::OnInit();
    SpriteRenderSystem::OnInit();
}

void Scene::onUpdate(Time dt)
{
    NativeScriptSystem::OnUpdate(dt);
    ParticleSystem::onUpdate(dt);
    PhysicsSystem::OnUpdate(dt);
    ColliderSystem::OnUpdate(dt);
    LightSystem::OnUpdate(dt);

    // Find a primary camera
    // TODO: implement multiple camera
    if(!findPrimaryCamera())
        return;

    // Draw SpriteRenderComponent
    {
        Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform, true);
        SpriteRenderSystem::onRender();
        Renderer2D::EndScene();
    }

    // Draw Particle system
    {
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
        Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform);
        ParticleSystem::onRender();
        Renderer2D::EndScene();
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
    }

    // Draw Light
    {
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::One, RenderCommand::BlendFactor::One);
        Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform);
        LightSystem::onRender();
        Renderer2D::EndScene();
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
    }
}

void Scene::onScenePlay()
{
    m_sceneState = SceneState::Play;

    NativeScriptSystem::OnScenePlay();
    PhysicsSystem::OnScenePlay();
}

void Scene::onScenePause()
{
    m_sceneState = SceneState::Pause;
}

void Scene::onSceneStop()
{
    m_sceneState = SceneState::Editor;

    NativeScriptSystem::OnSceneStop();
    PhysicsSystem::OnSceneStop();
}

void Scene::copySceneTo(Ref<Scene> &target)
{
    std::unordered_map<UUID, entt::entity> targetEnttMap{};
    //

    // Copy all entities by UUID
    m_registry.view<TagComponent>().each([&](auto ent, auto &tag) {
        RL_UNUSED(ent);
        Entity targetEnt = target->createEntity(tag.id, tag.tag);
        targetEnttMap[tag.id] = targetEnt.getEntityID();
    });

    // Copy components
    CopyComponent<TransformComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<SpriteRenderComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<CameraComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<NativeScriptComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<ParticleComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<RigidBody2DComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<BoxCollider2DComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<Joint2DComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    CopyComponent<LightComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);

    // Copy other states
    target->m_entNameToNumMap = m_entNameToNumMap;
}

void Scene::onImGuiRender()
{
    if(m_debugCamera)
    {
        DrawSceneCameraDebugWindow(m_mainCamera, m_mainCameraTransform);
    }

    if(m_debugPhysics)
    {
        PhysicsSystem::OnImGuiRender();
    }

    if(m_debugScene)
        DrawDebugSceneWindow(m_registry, this);
}

void Scene::onViewportResize(uint32_t width, uint32_t height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    auto view = m_registry.view<CameraComponent>();
    for(auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if(!cameraComponent.lockAspect)
            cameraComponent.camera.setViewportSize(width, height);
    }
}

// TODO: Improve the performance
Entity Scene::getEntityByUUID(UUID uuid)
{
    Entity target;
    //
    m_registry.view<TagComponent>().each([&](auto ent, auto &tag) {
        Entity entity{ent, this};
        if(tag.id == uuid)
            target = entity;
    });
    //
    return target;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Scene::findPrimaryCamera()
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

    return isAnyCamera;
}


} // namespace rl

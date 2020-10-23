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
//
#include <Rish/Effect/Particle/ParticleSystem.h>
//
#include <Rish/Physics/PhysicsSystem.h>
//
#include <Rish/Debug/DebugWindow.h>
#include <Rish/Utils/uuid.h>
//
#include <Rish/ImGui.h>

namespace rl{

int Scene::entityNumber = 0;

Scene::Scene()
{
    // TODO Remove me?
    m_registry.on_construct<ParticleComponent>().connect<entt::invoke<&ParticleComponent::init>>();
//    m_registry.on_construct<NativeScriptComponent>().connect<entt::invoke<&NativeScriptComponent::init>>();
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

void Scene::destroyEntity(const Entity &entity)
{
    m_registry.destroy(entity.getEntityID());
}

Entity Scene::duplicateEntity(Entity src)
{
    auto &tag = src.getComponent<TagComponent>().tag;
    m_entNameToNumMap[tag]++;
    auto ent = createEntity(fmt::format("{} ({})", tag, m_entNameToNumMap[tag]));

    CopyComponentToEntityIfExists<TransformComponent>(ent, src);
    CopyComponentToEntityIfExists<RenderComponent>(ent, src);
    CopyComponentToEntityIfExists<CameraComponent>(ent, src);
    CopyComponentToEntityIfExists<NativeScriptComponent>(ent, src);
    CopyComponentToEntityIfExists<RigidBody2DComponent>(ent, src);
    CopyComponentToEntityIfExists<BoxCollider2DComponent>(ent, src);
    CopyComponentToEntityIfExists<Joint2DComponent>(ent, src);
    CopyComponentToEntityIfExists<ParticleComponent>(ent, src);
}

void Scene::onRuntimeInit()
{
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc) {
        Entity ent{entityID, this};
        // Is bind
        if(nsc.instance)
        {
            nsc.instance->m_entity = Entity{entityID, this};
        }
        else
        {
            ScriptableManager::Bind(ent, nsc.scriptName);
        }
    });

    m_registry.view<RenderComponent>().each([=](auto ent, auto &render) {
        Entity entity{ent, this};

        if(render.init)
        {
            render.m_texture = Texture2D::LoadTextureVFS(render.texturePath);
            render.init = false;
        }
    });
}

void Scene::onEditorInit()
{
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc) {
        Entity ent{entityID, this};
        // Is bind
        if(nsc.instance)
        {
            nsc.instance->m_entity = Entity{entityID, this};
        }
        else
        {
            ScriptableManager::Bind(ent, nsc.scriptName);
        }
    });
}

void Scene::onUpdate(Time dt)
{
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc) {
        Entity ent{entityID, this};

        // TODO: Refactor me out for Editor
        // Is bind
        if(nsc.instance)
        {
            nsc.instance->m_entity = Entity{entityID, this};
        }
        else
        {
            ScriptableManager::Bind(ent, nsc.scriptName);
        }

        if(nsc.valid)
        {
            nsc.instance->onUpdate(dt);
        }
    });

    // Particle System update
    if(m_sceneState == SceneState::Play)
        ParticleSystem::onUpdate(m_registry, dt, m_sceneState);

    // Physics System update
    PhysicsSystem::onUpdate(m_registry, dt, m_sceneState);

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

    // Draw RenderComponent
    auto cameraGroup = m_registry.group<TransformComponent, RenderComponent>();
    Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform, true);
    {
        for (auto entity : cameraGroup) {
            Entity ent{entity, this};
            auto &transform = ent.getComponent<TransformComponent>();
            auto &render = ent.getComponent<RenderComponent>();

            if (render.m_texture) {
                if (transform.rotate != 0.f)
                    Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_texture,
                                                render.color, transform.rotate);
                else
                    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture,
                                         render.color);
            } else {
                if (transform.rotate != 0.f)
                    Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.color,
                                                transform.rotate);
                else
                    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
            }
        }
    }
    Renderer2D::EndScene();

    // Draw Particle system
    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
    Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform);
    {
        ParticleSystem::onRender(m_registry, m_sceneState);
    }
    Renderer2D::EndScene();
    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
}

void Scene::onScenePlay()
{
    m_sceneState = SceneState::Play;

    // Initialize the NativeScriptComponent
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        nsc.instance->onCreate();
        nsc.valid = true;
    });
    // Physics system initialize
    PhysicsSystem::onScenePlay(m_registry, m_sceneState);
}

void Scene::onScenePause()
{
    m_sceneState = SceneState::Pause;
}

void Scene::onSceneStop()
{
    m_sceneState = SceneState::Editor;

    // Destroy the NativeScriptComponent
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        Entity ent{entityID, this};
        nsc.instance->onDestroy();
        nsc.valid = false;
    });
    // Physics system clear
    PhysicsSystem::onSceneStop();
}

void Scene::copySceneTo(Ref<Scene> &target)
{
    std::unordered_map<UUID, entt::entity> targetEnttMap{};
    //
//    target->m_registry.clear();

    // Copy all entities by UUID
    // TODO: use m_registry.each ?
    auto view = m_registry.view<TagComponent>();
    for(auto ent : view)
    {
        auto &tag = view.get<TagComponent>(ent);
        Entity targetEnt = target->createEntity(tag.id, tag.tag);
        targetEnttMap[tag.id] = targetEnt.getEntityID();
    }

    // Copy components
    CopyComponent<TransformComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<RenderComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<CameraComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<NativeScriptComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<ParticleComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<RigidBody2DComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<BoxCollider2DComponent>(target->m_registry, m_registry, targetEnttMap, target);
    CopyComponent<Joint2DComponent>(target->m_registry, m_registry, targetEnttMap, target);
    
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
        PhysicsSystem::onImGuiRender();
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

} // namespace rl

#include <Rish/rlpch.h>
//
#include <Rish/Core/Time.h>
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
#include <Rish/Scene/SceneCamera.h>
// Systems
#include <Rish/Scene/System/SpriteRenderSystem.h>
#include <Rish/Effect/Particle/ParticleSystem.h>
#include <Rish/Collider/ColliderSystem.h>
#include <Rish/Physics/PhysicsSystem.h>
#include <Rish/Scene/System/NativeScriptSystem.h>
#include <Rish/Animation/Animation2DSystem.h>
//
#include <Rish/Debug/DebugWindow.h>
#include <Rish/Utils/uuid.h>
//
#include <Rish/ImGui/ImGui.h>
//

namespace rl {

int Scene::entityNumber = 0;

Scene::Scene()
    : physicsWorld(Vec2(0.0f, -9.8f))
{
    m_registry.on_construct<ParticleComponent>().connect<entt::invoke<&ParticleComponent::init>>();

    RL_CORE_INFO("Construct Scene");
}

Scene::~Scene()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Scene::addEntityToMap(const UUID& id, Entity entity)
{
    m_UUIDToEntityMap[id] = MakeRef<Entity>(entity);
}

void Scene::removeEntityFromMap(const UUID& id)
{
    RL_CORE_ASSERT(m_UUIDToEntityMap.count(id), "UUID {} not in map", id.to_string());

    m_UUIDToEntityMap.erase(id);
}

bool Scene::isEntityInMap(Entity entity)
{
    return m_UUIDToEntityMap.count(entity.getUUID());
}

bool Scene::isValidUUID(const UUID &id) const
{
    return m_UUIDToEntityMap.count(id);
}

///////////////////////////////////////////////

Entity Scene::createEntity(const std::string& name, const glm::vec3 &pos)
{
	Entity entity = { m_registry.create(), this };
	
	// Add necessary components
	entity.addComponent<TransformComponent>(pos);
	auto &tagComponent = entity.addComponent<TagComponent>();
	tagComponent.id = UUID();

	// Store in the entity map
    addEntityToMap(tagComponent.id, entity);

	// Naming the new entity
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

    tag = m_nameManager.getName(tag);
    m_nameManager.incrementName(tag);

	RL_CORE_TRACE("[Scene] Created entity {}", tag);
	return entity;
}

Entity Scene::createEntity(const UUID &id, const std::string &name)
{
    Entity entity = {m_registry.create(), this};

    // Add necessary components
    entity.addComponent<TransformComponent>();
    auto &tagComponent = entity.addComponent<TagComponent>();
    tagComponent.id = id;

    // Store in the entity map
    addEntityToMap(tagComponent.id, entity);

    // Naming the new entity
    auto &tag = tagComponent.tag;
    if(name.empty())
        tag = fmt::format("Entity {}", entityNumber);
    else
        tag = name;
    entityNumber++;

    tag = m_nameManager.getName(tag);
    m_nameManager.incrementName(tag);

    RL_CORE_TRACE("[Scene] Created entity {} by id {}", tag, id.to_string());
    return entity;
}

void Scene::EntityNameManager::incrementName(const std::string &name)
{
    std::string tmp{stripNumber(name)};
    //
    m_entNameToNumMap[tmp]++;
}

void Scene::EntityNameManager::decrementName(const std::string &name)
{
    std::string tmp{stripNumber(name)};
    //
    if(m_entNameToNumMap.count(tmp))
        m_entNameToNumMap[tmp]--;
}

std::string Scene::EntityNameManager::getName(const std::string &name)
{
    std::string tmp{stripNumber(name)};
    auto siz = m_entNameToNumMap[tmp];
    //
    if(siz)
        return fmt::format("{} ({})", tmp, siz);
    else
        return tmp;
}

std::string Scene::EntityNameManager::stripNumber(const std::string &str)
{
    std::string tmp{str};
    if(RE2::PartialMatch(tmp, R"(\(\d+\))"))
    {
        // Delete "(number)" part
        RE2::Replace(&tmp, R"( \(\d+\))", "");
    }
    return tmp;
}

void Scene::destroyEntity(Entity entity)
{
    removeEntityFromMap(entity.getUUID());

    m_nameManager.decrementName(entity.getName());

    NativeScriptSystem::OnDestroy(entity);

    m_registry.destroy(entity.getEntityID());
}

Entity Scene::duplicateEntity(Entity src)
{
    auto &tag = src.getComponent<TagComponent>().tag;

    auto ent = createEntity(m_nameManager.stripNumber(tag));

    CopyComponentToEntityIfExists<TransformComponent>(ent, src);
    CopyComponentToEntityIfExists<SpriteRenderComponent>(ent, src);
    CopyComponentToEntityIfExists<CameraComponent>(ent, src);
    CopyComponentToEntityIfExists<NativeScriptComponent>(ent, src);
    CopyComponentToEntityIfExists<RigidBody2DComponent>(ent, src);
    CopyComponentToEntityIfExists<BoxCollider2DComponent>(ent, src);
    CopyComponentToEntityIfExists<Joint2DComponent>(ent, src);
    CopyComponentToEntityIfExists<ParticleComponent>(ent, src);
    CopyComponentToEntityIfExists<Animation2DComponent>(ent, src);

    return ent;
}

void Scene::onRuntimeInit()
{
    NativeScriptSystem::OnInit();
    SpriteRenderSystem::OnInit();

    // Store entities into map
    m_registry.each([&](auto ent) {
        Entity entity{ent, this};
        m_UUIDToEntityMap[entity.getUUID()] = MakeRef<Entity>(entity);
    });
}

void Scene::onEditorInit()
{
    NativeScriptSystem::OnInit();
    SpriteRenderSystem::OnInit();
    
    // Store entities into map
    m_registry.each([&](auto ent) {
        Entity entity{ent, this};
        m_UUIDToEntityMap[entity.getUUID()] = MakeRef<Entity>(entity);
    });
}

void Scene::onUpdate(Time dt)
{
    NativeScriptSystem::OnUpdate(dt);
    ParticleSystem::onUpdate(dt);
    PhysicsSystem::OnUpdate(dt);
    ColliderSystem::OnUpdate(dt);
    Animation2DSystem::OnUpdate(dt);
}

void Scene::onRender()
{
    // Find a primary camera
    // TODO: implement multiple camera
    if(!findPrimaryCamera())
        return;

    // Draw SpriteRenderComponent
    {
        Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform, true);
        SpriteRenderSystem::onRender();
        Animation2DSystem::OnRender();
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
}

void Scene::onScenePlay()
{
    m_sceneState = SceneState::Play;

    NativeScriptSystem::OnScenePlay();
    PhysicsSystem::OnScenePlay();
    Animation2DSystem::OnScenePlay();
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
    Animation2DSystem::OnSceneStop();
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
    CopyComponent<Animation2DComponent>(target->m_registry, m_registry, targetEnttMap, target.get(), this);
    
    // Copy other states
    target->m_nameManager = m_nameManager;
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
    {
        DrawDebugSceneWindow(m_registry, this);
    }

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
    if(!isValidUUID(uuid))
    {
        RL_CORE_ERROR("UUID {} is not in map", uuid.to_string());
        return Entity{entt::null, this};
    }
    else
        return *m_UUIDToEntityMap[uuid];
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

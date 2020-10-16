#include <Rish/rlpch.h>
//
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/Framebuffer.h>
#include <Rish/Renderer/RendererCommand.h>
//
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/Utils.h>
//
#include <Rish/Effect/Particle/ParticleSystem.h>
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
    CopyComponentToEntityIfExists<ParticleComponent>(ent, src);
}

void Scene::onUpdate(Time dt)
{
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc) {
        nsc.instance->m_entity = Entity{entityID, this};
        if(nsc.valid)
        {
            nsc.instance->onUpdate(dt);
        }
    });

    // hax boxCollider change coordinate
    if(m_sceneState == SceneState::Play) {
        auto group4 = m_registry.view<TransformComponent, BoxCollider2DComponent>();
        for (auto entity : group4) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();

            // if it has boxCollider component then update Physics coordinate
            if (mapBoxColliderObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                auto &boxc = mapBoxColliderObj[UUID];
                phy->position.x = transform.translate.x + boxc->x;
                phy->position.y = transform.translate.y + boxc->y;
                phy->wh.x = boxc->w;
                phy->wh.y = boxc->h;
            }
        }

        PhysicsWorld.Step(dt);

        // if it has BoxCollider component, return to transform coordinate
        auto group2 = m_registry.view<TransformComponent, BoxCollider2DComponent>();
        for (auto entity : group2) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();
            if (mapBoxColliderObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                auto &boxc = mapBoxColliderObj[UUID];
                float offset_x = phy->position.x - (transform.translate.x + boxc->x);
                float offset_y = phy->position.y - (transform.translate.y + boxc->y);
                transform.translate.x += offset_x;
                transform.translate.y += offset_y;
            }
        }

        // RigidBody2D Component
        // update physics object to world
        auto group3 = m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group3) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();
            auto &rigidbody2D = ent.getComponent<RigidBody2DComponent>();

            if (mapPhysicsObj.count(UUID) && !mapBoxColliderObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                // has boxCollider Component
                // after physics calculate and update position
                transform.translate.x = phy->position.x;
                transform.translate.y = phy->position.y;
                transform.scale.x = phy->wh.x;
                transform.scale.y = phy->wh.y;
                transform.rotate = glm::degrees(phy->angle);
                // update rigidbody2D component value
                rigidbody2D.angularVelocity = phy->torque;
                rigidbody2D.angle = phy->angle;
                rigidbody2D.velocity = phy->velocity;
                rigidbody2D.force = phy->force;
                rigidbody2D.torque = phy->torque;
                rigidbody2D.friction = phy->friction;
            }
        }
    }
    if(m_sceneState == SceneState::Pause)
    {
        auto group3 = m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group3) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();

            if (mapPhysicsObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                // has boxCollider Component
                // update position for physics
                phy->position.x = transform.translate.x;
                phy->position.y = transform.translate.y;
                phy->wh.x = transform.scale.x;
                phy->wh.y = transform.scale.x;
            }
        }
    }

    // Particle System update
    if(m_sceneState == SceneState::Play)
        ParticleSystem::onUpdate(m_registry, dt, m_sceneState);

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
    Renderer2D::BeginScene(m_mainCamera, m_mainCameraTransform);
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
        Entity ent{entityID, this};
        nsc.instance->onCreate();
        nsc.valid = true;
    });

    // RigidBody2D Component
    // add physics object to world
    auto group = m_registry.view<TransformComponent, RigidBody2DComponent>();
    for(auto entity : group)
    {
        Entity ent{entity, this};
        auto &UUID = ent.getComponent<TagComponent>().id;
        auto &transform = ent.getComponent<TransformComponent>();
        auto &rigidbody2D = ent.getComponent<RigidBody2DComponent>();

        // if UUID not exist, append new physics obj
        if(!mapPhysicsObj.count(UUID))
        {
            auto physicsObj = MakeRef<RigidBody2D>(Vec2(transform.translate.x, transform.translate.y), Vec2(transform.scale.x, transform.scale.y), rigidbody2D.mass);
            mapPhysicsObj[UUID] = physicsObj;
            PhysicsWorld.Add(physicsObj);
        }
    }

    // BoxCollider Component
    // add boxCollider component
    auto group3 = m_registry.view<TransformComponent, BoxCollider2DComponent>();
    for(auto entity : group3)
    {
        Entity ent{entity, this};

        auto &UUID = ent.getComponent<TagComponent>().id;
        auto &boxCollider = ent.getComponent<BoxCollider2DComponent>();
        auto &transform = ent.getComponent<TransformComponent>();

        // if UUID not exist, append box obj

        if(!mapBoxColliderObj.count(UUID)) {
            auto box = MakeRef<Box>(boxCollider.x, boxCollider.y, boxCollider.w, boxCollider.h);
            mapBoxColliderObj[UUID] = box;
        }
    }
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

    // clear physics object
    PhysicsWorld.Clear();
    mapPhysicsObj.clear();
    mapBoxColliderObj.clear();
}

void Scene::copySceneTo(Ref<Scene> &target)
{
    std::unordered_map<UUID, entt::entity> targetEnttMap{};
    //
    target->m_registry.clear();

    // Copy all entities by UUID
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

    // Copy other states
    target->m_entNameToNumMap = m_entNameToNumMap;
    target->mapPhysicsObj = mapPhysicsObj;
}

void Scene::onImGuiRender()
{
    if(m_debugCamera)
    {
        DrawSceneCameraDebugWindow(m_mainCamera, m_mainCameraTransform);
    }

    if(m_debugPhysics)
    {
        ImGui::Begin("Physics");
        for(auto && [uuid, body] : mapPhysicsObj)
        {
            if(ImGui::TreeNode(uuid.to_string().c_str()))
            {
                ImGui::Text("%.f %.f %.f %.f\n", body->position.x, body->position.y, body->wh.x, body->wh.y);
                ImGui::Text("mass: %.f\n", body->mass);
                ImGui::TreePop();
            }
        }
        ImGui::End();

        ImGui::Begin("Collider");
        for(auto && [uuid, box] : mapBoxColliderObj)
        {
            if(ImGui::TreeNode(uuid.to_string().c_str()))
            {
                ImGui::Text("%.f %.f %.f %.f\n", box->x, box->y, box->w, box->h);
                ImGui::TreePop();
            }
        }
        ImGui::End();

        ImGui::Begin("State");
            ImGui::Text("%d", this->getSceneState());
        ImGui::End();
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

} // namespace rl

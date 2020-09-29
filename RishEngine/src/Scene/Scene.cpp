#include <Rish/rlpch.h>
//
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/Framebuffer.h>
//
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/Utils.h>
//
#include <Rish/Utils/uuid.h>
//
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

template<typename T>
void CopyComponentToEntityIfExists(Entity dst, Entity src)
{
    if(src.hasComponent<T>())
    {
        auto &dstComponent = dst.addComponent<T>();
        dstComponent = src.getComponent<T>();
    }
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
}

void Scene::onUpdate(Time dt)
{
    m_registry.view<NativeScriptComponent>().each([=](auto entity, auto &nsc) {
        RL_UNUSED(entity);
        if(nsc.instance) {
            nsc.instance->onUpdate(dt);
        }
    });

    if(m_sceneState == SceneState::Play)
        PhysicsWorld.Step(dt);

    // RigidBody2D Component
    // update physics object to world
    auto group2 = m_registry.view<TransformComponent, RigidBody2DComponent>();
    for(auto entity : group2)
    {
        Entity ent{entity, this};
        auto &UUID = ent.getComponent<TagComponent>().id;
        auto &transform = ent.getComponent<TransformComponent>();
        auto &rigidbody2D = ent.getComponent<RigidBody2DComponent>();

        if(mapPhysics_obj.count(UUID))
        {
            auto &phy = mapPhysics_obj[UUID];
            // after physics calculate and update position
            transform.translate.x = phy->position.x;
            transform.translate.y = phy->position.y;
            transform.scale.x = phy->wh.x;
            transform.scale.y = phy->wh.y;
            // update rigidbody2D component value
            rigidbody2D.angularVelocity = phy->torque;
            rigidbody2D.angle = phy->angle;
            rigidbody2D.velocity = phy->velocity;
            rigidbody2D.force = phy->force;
            rigidbody2D.torque = phy->torque;
            rigidbody2D.friction = phy->friction;
        }
    }

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
        {
            if(transform.rotate != 0.f)
                Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color, transform.rotate);
            else
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color);
        }
        else
        {
            if(transform.rotate != 0.f)
                Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.color, transform.rotate);
            else
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
        }
    }
    Renderer2D::EndScene();
}

void Scene::onScenePlay()
{
    m_sceneState = SceneState::Play;

    // Initialize the NativeScriptComponent
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        if (!nsc.instance) {
            nsc.instance = nsc.newScript();
            nsc.instance->m_entity = Entity{entityID, this};
            nsc.instance->onCreate();
        }
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

        //if the UUID exist, append new physics obj
        if(!mapPhysics_obj.count(UUID))
        {
            auto physicsObj = MakeRef<RigidBody2D>(Vec2(transform.translate.x, transform.translate.y), Vec2(transform.scale.x, transform.scale.y), rigidbody2D.mass);
            mapPhysics_obj[UUID] = physicsObj;
            PhysicsWorld.Add(physicsObj);
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
        nsc.instance->onDestroy();
        nsc.deleteScript(&nsc);
    });

    PhysicsWorld.Clear();
    mapPhysics_obj.clear();
}

void Scene::copySceneTo(Ref<Scene> &target)
{
    std::unordered_map<UUID, entt::entity> targetEnttMap;
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
    CopyComponent<TransformComponent>(target->m_registry, m_registry, targetEnttMap);
    CopyComponent<RenderComponent>(target->m_registry, m_registry, targetEnttMap);
    CopyComponent<CameraComponent>(target->m_registry, m_registry, targetEnttMap);
    CopyComponent<NativeScriptComponent>(target->m_registry, m_registry, targetEnttMap);
    CopyComponent<RigidBody2DComponent>(target->m_registry, m_registry, targetEnttMap);
    // Copy other states
    target->m_entNameToNumMap = m_entNameToNumMap;
    target->mapPhysics_obj = mapPhysics_obj;
}

void Scene::onImGuiRender()
{
    if(m_debugCamera)
    {
        ImGui::Begin("Scene Debug");
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
        ImGui::Text("Size = %.2f", m_mainCamera.m_orthoSize);
        ImGui::Text("Near = %.2f, Far = %.2f", m_mainCamera.m_orthoNear, m_mainCamera.m_orthoFar);
        ImGui::Text("Aspect = %.2f", m_mainCamera.m_aspect);
        ImGui::End();
    }

    if(m_debugPhysics)
    {
        ImGui::Begin("Physics");
        for(auto && [uuid, body] : mapPhysics_obj)
        {
            if(ImGui::TreeNode(uuid.to_string().c_str()))
            {
                ImGui::Text("%.f %.f %.f %.f", body->position.x, body->position.y, body->wh.x, body->wh.y);
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    if(m_debugCameraComponent)
    {
        ImGui::Begin("CameraComponent");
        auto view = m_registry.view<CameraComponent>();
        for(auto entity : view)
        {
            Entity ent{entity, this};
            //
            auto &tag = ent.getComponent<TagComponent>();
            auto &transform = ent.getComponent<TransformComponent>();
            if(ImGui::TreeNode(tag.id.to_string().c_str()))
            {
                ImGui::Text("transform = %.2f %.2f %.2f", transform.translate.x, transform.translate.y, transform.translate.z);
                ImGui::Text("scale     = %.2f %.2f %.2f", transform.scale.x, transform.scale.y, transform.scale.z);
                ImGui::Text("rotate    = %.2f", transform.rotate);
                ImGui::TreePop();
            }
        }
        ImGui::End();
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

} // namespace rl

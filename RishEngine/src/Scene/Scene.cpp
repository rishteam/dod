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

    RL_CORE_TRACE("[Scene] Created entity {} by id {}", tag, id.to_string());
    return entity;
}

void Scene::destroyEntity(const Entity &entity)
{
    m_registry.destroy(entity.getEntityID());
}

void Scene::onUpdate(Time dt)
{
    m_registry.view<NativeScriptComponent>().each([=](auto entity, auto &nsc) {
        if(nsc.instance) {
            nsc.instance->onUpdate(dt);
        }
    });

    if(m_sceneState == SceneState::Play) {

        // Component data to Physics engine
        auto group = m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();
            auto &rigidbody2D = ent.getComponent<RigidBody2DComponent>();
            // BoxCollider & RigidBody2D
            if (mapBoxColliderObj.count(UUID) && mapPhysicsObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                auto &boxc = mapBoxColliderObj[UUID];

                phy->position.x = transform.translate.x + boxc->x;
                phy->position.y = transform.translate.y + boxc->y;
                phy->wh.x = boxc->w;
                phy->wh.y = boxc->h;

                phy->angle = glm::radians(transform.rotate);

                phy->mass = rigidbody2D.mass;
                phy->friction = rigidbody2D.friction;
                phy->velocity = rigidbody2D.velocity;
                phy->force = rigidbody2D.force;
                phy->isCollider = true;
            }
            // Only RigidBody2D
            else if(mapPhysicsObj.count(UUID))
            {
                auto &phy = mapPhysicsObj[UUID];
                phy->position.x = transform.translate.x;
                phy->position.y = transform.translate.y;
                phy->wh.x = transform.scale.x;
                phy->wh.y = transform.scale.y;

                phy->angle = glm::radians(transform.rotate);
                phy->mass = rigidbody2D.mass;
                phy->friction = rigidbody2D.friction;
                phy->velocity = rigidbody2D.velocity;
                phy->force = rigidbody2D.force;
                phy->isCollider = false;
            }
        }

        // Joint Component to Physics engine joint
        auto group2 = m_registry.view<TransformComponent, Joint2DComponent>();
        for (auto entity : group2) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &jitComponent = ent.getComponent<Joint2DComponent>();
            if(mapJointObj.count(UUID))
            {
                auto &jit = mapJointObj[UUID];
                jit->Set(mapPhysicsObj[jitComponent.rigidBody1], mapPhysicsObj[jitComponent.rigidBody2], jitComponent.anchor);
            }
        }

        // Physics simulate
        PhysicsWorld.Step(dt);

        // Physics engine data to Component
        // BoxCollider & RigidBody2D
        auto group3 = m_registry.view<TransformComponent, RigidBody2DComponent, BoxCollider2DComponent>();
        for (auto entity : group3) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();
            auto &rigidbody2D = ent.getComponent<RigidBody2DComponent>();
            auto &boxcollider2D = ent.getComponent<BoxCollider2DComponent>();

            if (mapPhysicsObj.count(UUID) && mapBoxColliderObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                auto &boxc = mapBoxColliderObj[UUID];
                float offset_x = phy->position.x - (transform.translate.x + boxc->x);
                float offset_y = phy->position.y - (transform.translate.y + boxc->y);

                transform.translate.x += offset_x;
                transform.translate.y += offset_y;
                transform.rotate = glm::degrees(phy->angle);

                rigidbody2D.angularVelocity = phy->angularVelocity;
                rigidbody2D.angle = phy->angle;
                rigidbody2D.velocity = phy->velocity;
                rigidbody2D.force = phy->force;
                rigidbody2D.torque = phy->torque;
                rigidbody2D.friction = phy->friction;
                rigidbody2D.mass = phy->mass;
            }
        }

        // only RigidBody2D
        auto group4 = m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group4) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();
            auto &rigidbody2D = ent.getComponent<RigidBody2DComponent>();

            if(!mapBoxColliderObj.count(UUID) && mapPhysicsObj.count(UUID))
            {
                auto &phy = mapPhysicsObj[UUID];
                transform.translate.x = phy->position.x;
                transform.translate.y = phy->position.y ;
                transform.scale.x = phy->wh.x;
                transform.scale.y = phy->wh.y;
                transform.rotate = glm::degrees(phy->angle);

                rigidbody2D.angularVelocity = phy->angularVelocity;
                rigidbody2D.angle = phy->angle;
                rigidbody2D.velocity = phy->velocity;
                rigidbody2D.force = phy->force;
                rigidbody2D.torque = phy->torque;
                rigidbody2D.friction = phy->friction;
                rigidbody2D.mass = phy->mass;
            }
        }

        // TODO: Joint Data in physics engine to Component
        auto group6 = m_registry.view<TransformComponent, Joint2DComponent>();
        for (auto entity : group6) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();
            auto &jitComponent = ent.getComponent<Joint2DComponent>();

            if(mapJointObj.count(UUID))
            {
                auto jit = mapJointObj[UUID];
                jitComponent.r1 = jit->r1;
                jitComponent.r2 = jit->r2;
                jitComponent.P = jit->P;
                jitComponent.bias = jit->bias;
                jitComponent.biasFactor = jit->biasFactor;
                jitComponent.softness = jit->softness;
            }
        }
    }
    if(m_sceneState == SceneState::Pause)
    {
        auto group5 = m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group5) {
            Entity ent{entity, this};
            auto &UUID = ent.getComponent<TagComponent>().id;
            auto &transform = ent.getComponent<TransformComponent>();

            // has boxCollider Component
            if (mapPhysicsObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];

                // update position for physics
                phy->position.x = transform.translate.x;
                phy->position.y = transform.translate.y;
                phy->wh.x = transform.scale.x;
                phy->wh.y = transform.scale.x;
            }
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
    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        if (!nsc.instance) {
            nsc.instance = nsc.newScript();
            nsc.instance->m_entity = Entity{entityID, this};
            nsc.instance->onCreate();
        }
    });

    // RigidBody2D Component
    // add physics object in physics engine
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

    // BoxCollider2DComponent Component
    // add collider in physics engine
    auto group2 = m_registry.view<TransformComponent, BoxCollider2DComponent>();
    for(auto entity : group2)
    {
        Entity ent{entity, this};
        auto &UUID = ent.getComponent<TagComponent>().id;
        auto &transform = ent.getComponent<TransformComponent>();
        auto &boxc = ent.getComponent<BoxCollider2DComponent>();
        // BoxCollider Component
        if(!mapBoxColliderObj.count(UUID)) {
            auto box = MakeRef<Box>(boxc.x, boxc.y, boxc.w, boxc.h);
            mapBoxColliderObj[UUID] = box;
        }
    }

    // Joint2DComponent Component
    // add joint in physics engine
    auto group3 = m_registry.view<TransformComponent, Joint2DComponent>();
    for(auto entity : group3)
    {
        Entity ent{entity, this};
        auto &UUID = ent.getComponent<TagComponent>().id;
        auto &transform = ent.getComponent<TransformComponent>();
        auto &jit = ent.getComponent<Joint2DComponent>();
        // BoxCollider Component
        if(!mapJointObj.count(UUID)) {
            auto jit = MakeRef<Joint>();
            PhysicsWorld.AddJoints(jit);
            mapJointObj[UUID] = jit;
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

    m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        nsc.deleteScript(&nsc);
    });

    //clear physics object
    PhysicsWorld.Clear();
    mapPhysicsObj.clear();
    mapBoxColliderObj.clear();
    mapJointObj.clear();
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
    CopyComponent<BoxCollider2DComponent>(target->m_registry, m_registry, targetEnttMap);
    CopyComponent<Joint2DComponent>(target->m_registry, m_registry, targetEnttMap);
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

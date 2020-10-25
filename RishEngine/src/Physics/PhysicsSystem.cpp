#include <Rish/Physics/PhysicsSystem.h>

namespace rl {

Ref<Scene> PhysicsSystem::s_Scene;

void PhysicsSystem::onUpdate(const Ref<Scene>& scene, float dt)
{
    auto &registry = scene->m_registry;
    const auto &state = scene->getSceneState();
    onUpdate(registry, dt, state);
}

void PhysicsSystem::onInit(const Ref <Scene> &scene)
{
   s_Scene = scene;
}

void PhysicsSystem::onUpdateNewPhysicsObject(entt::registry& registry, Scene::SceneState state)
{
    auto &physicsWorld = s_Scene->physicsWorld;
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

    auto &StatePhysicsObj = s_Scene->StatePhysicsObj;
    auto &StateBoxColliderObj = s_Scene->StateBoxColliderObj;
    auto &StateJointObj = s_Scene->StateJointObj;

    // All state Clear
    for(auto statePhy : StatePhysicsObj)
    {
        statePhy.second = false;
    }
    for(auto stateboxc : StateBoxColliderObj)
    {
        stateboxc.second = false;
    }
    for(auto statejit : StateJointObj)
    {
        statejit.second = false;
    }

    // RigidBody2D Component
    // add physics object in physics engine
    auto group = registry.view<TransformComponent, RigidBody2DComponent>();
    for(auto entity : group)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);

        // if UUID not exist, append new physics obj
        if(!mapPhysicsObj.count(UUID))
        {
            auto physicsObj = MakeRef<RigidBody2D>(Vec2(transform.translate.x, transform.translate.y), Vec2(transform.scale.x, transform.scale.y), rigidbody2D.mass);
            mapPhysicsObj[UUID] = physicsObj;
            // Add Force on Point
            auto attx = transform.translate.x + rigidbody2D.attachPoint.x;
            auto atty = transform.translate.y + rigidbody2D.attachPoint.y;
            physicsObj->AddForce(rigidbody2D.force, Vec2(attx, atty));
            physicsWorld.Add(physicsObj);
            StatePhysicsObj.push_back(std::make_pair(UUID, true));
        }
    }

    // BoxCollider2DComponent Component
    // add collider in physics engine
    auto group2 = registry.view<TransformComponent, BoxCollider2DComponent>();
    for(auto entity : group2)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &boxc = registry.get<BoxCollider2DComponent>(entity);
        // BoxCollider Component
        if(!mapBoxColliderObj.count(UUID)) {
            auto box = MakeRef<Box>(boxc.x, boxc.y, boxc.w, boxc.h);
            mapBoxColliderObj[UUID] = box;
            StateBoxColliderObj.push_back(std::make_pair(UUID, true));
        }
    }

    // Joint2DComponent Component
    // add joint in physics engine
    auto group3 = registry.view<TransformComponent, Joint2DComponent>();
    for(auto entity : group3)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &jit = registry.get<Joint2DComponent>(entity);
        // BoxCollider Component
        if(!mapJointObj.count(UUID)) {
            auto jit = MakeRef<Joint>();
            physicsWorld.AddJoints(jit);
            mapJointObj[UUID] = jit;
            StateJointObj.push_back(std::make_pair(UUID, true));
        }
    }
}


void PhysicsSystem::onCleanPhysicObject()
{
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapJointObj = s_Scene->mapJointObj;

    auto &StatePhysicsObj = s_Scene->StatePhysicsObj;
    auto &StateBoxColliderObj = s_Scene->StateBoxColliderObj;
    auto &StateJointObj = s_Scene->StateJointObj;

    for(auto statePhy : StatePhysicsObj)
    {
        if (!statePhy.second)
        {
            mapPhysicsObj.erase(mapPhysicsObj.find(statePhy.first), mapPhysicsObj.end());
        }
    }
    for(auto stateboxc : StateBoxColliderObj)
    {
        if (!stateboxc.second)
        {
            mapBoxColliderObj.erase(mapBoxColliderObj.find(stateboxc.first), mapBoxColliderObj.end());
        }
    }
    for(auto statejit : StateJointObj)
    {
        if (!statejit.second)
        {
            mapJointObj.erase(mapJointObj.find(statejit.first), mapJointObj.end());
        }
    }

    std::remove_if(StatePhysicsObj.begin(),
                 StatePhysicsObj.end(),
                 [](auto tmp)
                 {
                    if(!tmp.second)
                    {
                        return true;
                    }
                 });
    std::remove_if (StateBoxColliderObj.begin(),
                  StateBoxColliderObj.end(),
                  [](auto tmp)
                  {
                      if(!tmp.second)
                      {
                          return true;
                      }
                  });
    std::remove_if (StateJointObj.begin(),
                    StateJointObj.end(),
                    [](auto tmp)
                    {
                        if(!tmp.second)
                        {
                            return true;
                        }
                    });
}

void PhysicsSystem::onUpdate(entt::registry& registry, float dt, Scene::SceneState state)
{
    auto &physicsWorld = s_Scene->physicsWorld;
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

    if (state == Scene::SceneState::Play)
    {
        onUpdateNewPhysicsObject(registry, state);
        onCleanPhysicObject();
        // update BoxCollider2D component data to engine
        auto view = registry.view<TransformComponent, BoxCollider2DComponent>();
        for (auto entity : view)
        {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &boxcollider2D = registry.get<BoxCollider2DComponent>(entity);

            if (mapBoxColliderObj.count(UUID))
            {
                auto &boxc = mapBoxColliderObj[UUID];
                boxc->x = boxcollider2D.x;
                boxc->y = boxcollider2D.y;
                boxc->w = boxcollider2D.w;
                boxc->h = boxcollider2D.h;
            }
        }

        // update RigidBody2D Component data to engine
        auto view2 = registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : view2)
        {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);

            // BoxCollider & RigidBody2D
            if (mapBoxColliderObj.count(UUID) && mapPhysicsObj.count(UUID))
            {
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
                phy->attachPoint = rigidbody2D.attachPoint;
                phy->keepingForce = rigidbody2D.keepingForce;
                phy->isCollider = true;
            }
            // Only RigidBody2D
            else if (mapPhysicsObj.count(UUID))
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
                phy->attachPoint = rigidbody2D.attachPoint;
                phy->keepingForce = rigidbody2D.keepingForce;
                phy->isCollider = false;
            }
        }

        // Joint Component to Physics engine joint
        auto group2 = registry.view<TransformComponent, Joint2DComponent>();
        for (auto entity : group2)
        {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &jitComponent = registry.get<Joint2DComponent>(entity);
            auto &mapJointObj = s_Scene->mapJointObj;

            if (mapJointObj.count(UUID))
            {
                auto &jit = mapJointObj[UUID];
                jit->Set(mapPhysicsObj[jitComponent.rigidBody1], mapPhysicsObj[jitComponent.rigidBody2],
                         jitComponent.anchor);
            }
        }

        // Physics simulate
        physicsWorld.Step(dt);

        // Physics engine data to Component
        // BoxCollider & RigidBody2D
        auto group3 = registry.view<TransformComponent, RigidBody2DComponent, BoxCollider2DComponent>();
        for (auto entity : group3)
        {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);
            auto &boxcollider2D = registry.get<BoxCollider2DComponent>(entity);

            if (mapPhysicsObj.count(UUID) && mapBoxColliderObj.count(UUID))
            {
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
                rigidbody2D.torque = phy->torque;
                rigidbody2D.friction = phy->friction;
                rigidbody2D.force = phy->force;
                rigidbody2D.mass = phy->mass;
                rigidbody2D.attachPoint = phy->attachPoint;
                rigidbody2D.keepingForce = phy->keepingForce;
            }
        }

        // only RigidBody2D
        auto group4 = registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group4)
        {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);

            if (!mapBoxColliderObj.count(UUID) && mapPhysicsObj.count(UUID))
            {
                auto &phy = mapPhysicsObj[UUID];
                transform.translate.x = phy->position.x;
                transform.translate.y = phy->position.y;
                transform.scale.x = phy->wh.x;
                transform.scale.y = phy->wh.y;
                transform.rotate = glm::degrees(phy->angle);

                rigidbody2D.angularVelocity = phy->angularVelocity;
                rigidbody2D.angle = phy->angle;
                rigidbody2D.velocity = phy->velocity;
                rigidbody2D.torque = phy->torque;
                rigidbody2D.friction = phy->friction;
                rigidbody2D.force = phy->force;
                rigidbody2D.mass = phy->mass;
                rigidbody2D.attachPoint = phy->attachPoint;
                rigidbody2D.keepingForce = phy->keepingForce;
            }
        }
        // joint
        auto group6 = registry.view<TransformComponent, Joint2DComponent>();
        for (auto entity : group6)
        {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &jitComponent = registry.get<Joint2DComponent>(entity);

            if (mapJointObj.count(UUID))
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
}

void PhysicsSystem::onScenePlay(entt::registry& registry, Scene::SceneState state)
{
    onUpdateNewPhysicsObject(registry, state);
}

void PhysicsSystem::onSceneStop()
{
    auto &physicsWorld = s_Scene->physicsWorld;
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

    // clear physics object
    physicsWorld.Clear();
    mapPhysicsObj.clear();
    mapBoxColliderObj.clear();
    mapJointObj.clear();
}

void PhysicsSystem::onImGuiRender()
{
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

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
}

}
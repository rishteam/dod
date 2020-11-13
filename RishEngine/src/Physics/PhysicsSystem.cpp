#include <Rish/Physics/PhysicsSystem.h>

namespace rl {

static float accumulateTime = 0.0f;
static float MS_PER_UPDATE = 1.0f / 120.0f;

Ref<Scene> PhysicsSystem::s_Scene;

void PhysicsSystem::RegisterScene(const Ref <Scene> &scene)
{
   s_Scene = scene;
}

void PhysicsSystem::OnUpdate(float dt)
{
    // Physics World
    auto &physicsWorld = s_Scene->physicsWorld;
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

    // State
    auto state = s_Scene->getSceneState();
    auto &registry = s_Scene->m_registry;

    if (state == Scene::SceneState::Play)
    {
        // Update and Clear nonExists Physics Object
        UpdateNewPhysicsObject(registry, state);
        // PhysicsWorld update with delta Time
        PhysicsWorldUpdate(dt);

        // Physics engine data to Component
        // BoxCollider & RigidBody2D
        auto group = registry.view<TransformComponent, RigidBody2DComponent, BoxCollider2DComponent>();
        for (auto entity : group) {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);
            auto &boxcollider2D = registry.get<BoxCollider2DComponent>(entity);

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
                rigidbody2D.torque = phy->torque;
                rigidbody2D.friction = phy->friction;
                rigidbody2D.force = phy->force;
                rigidbody2D.mass = phy->mass;
                rigidbody2D.attachPoint = phy->attachPoint;
                rigidbody2D.keepingForce = phy->keepingForce;
            }
        }
        // Only RigidBody2D
        auto group2 = registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group2) {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);

            if (!mapBoxColliderObj.count(UUID) && mapPhysicsObj.count(UUID)) {
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
            // Restrict Rotation
            if (rigidbody2D.RestrictRotation) {
                transform.rotate = 0.0f;
            }
        }
        // Joint
        auto group3 = registry.view<TransformComponent, Joint2DComponent>();
        for (auto entity : group3) {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &jitComponent = registry.get<Joint2DComponent>(entity);

            if (mapJointObj.count(UUID)) {
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

void PhysicsSystem::OnScenePlay()
{
    auto &registry = s_Scene->m_registry;
    auto state = s_Scene->getSceneState();
    InitPhysicsObject(registry, state);
}

void PhysicsSystem::OnSceneStop()
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

void PhysicsSystem::OnImGuiRender()
{
    // Physics World
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;
    // State Record check it exists
    auto &statePhysicsObject = s_Scene->StatePhysicsObj;
    auto &stateBoxColliderObject = s_Scene->StateBoxColliderObj;
    auto &stateJointObject = s_Scene->StateJointObj;

    ImGui::Begin("PhysicsWorld");
    ImGui::Text("PhysicsWorld Counter: %d", mapPhysicsObj.size());
    for(auto && [uuid, body] : mapPhysicsObj)
    {
        if(ImGui::TreeNode(uuid.to_string().c_str()))
        {
            ImGui::Text("uuid: %s", uuid.to_string().c_str());
            ImGui::Text("%.f %.f %.f %.f\n", body->position.x, body->position.y, body->wh.x, body->wh.y);
            ImGui::Separator();
        }
    }
    ImGui::End();

    ImGui::Begin("PhysicsState");
    ImGui::Text("PhysicsState Counter: %d", statePhysicsObject.size());
    for(auto && [uuid, body] : statePhysicsObject)
    {
        if(ImGui::TreeNode(uuid.to_string().c_str()))
        {
            ImGui::Text("%s %d\n", uuid.to_string().c_str(), body);
            ImGui::Separator();
        }
    }
    ImGui::End();

    ImGui::Begin("Collider");
    ImGui::Text("Collider Counter: %d", mapBoxColliderObj.size());
    for(auto && [uuid, box] : mapBoxColliderObj)
    {
        if(ImGui::TreeNode(uuid.to_string().c_str()))
        {
            ImGui::Text("uuid: %s", uuid.to_string().c_str());
            ImGui::Text("%.f %.f %.f %.f\n", box->x, box->y, box->w, box->h);
            ImGui::TreePop();
        }
    }
    ImGui::End();

    ImGui::Begin("ColliderState");
    ImGui::Text("ColliderState Counter: %d", stateBoxColliderObject.size());
    for(auto && [uuid, body] : stateBoxColliderObject)
    {
        if(ImGui::TreeNode(uuid.to_string().c_str()))
        {
            ImGui::Text("%s %d\n", uuid.to_string().c_str(), body);
            ImGui::Separator();
        }
    }
    ImGui::End();


    ImGui::Begin("Joint");
    ImGui::Text("Joint Counter: %d", mapJointObj.size());
    for(auto && [uuid, body] : mapJointObj)
    {
        if(ImGui::TreeNode(uuid.to_string().c_str()))
        {
            ImGui::Text("%s \n", uuid.to_string().c_str());
            ImGui::Separator();
        }
    }
    ImGui::End();


    ImGui::Begin("JointState");
    ImGui::Text("JointState Counter: %d", stateJointObject.size());
    for(auto && [uuid, body] : stateJointObject)
    {
        if(ImGui::TreeNode(uuid.to_string().c_str()))
        {
            ImGui::Text("%s %d\n", uuid.to_string().c_str(), body);
            ImGui::Separator();
        }
    }
    ImGui::End();
}

void PhysicsSystem::PhysicsWorldUpdate(float dt)
{
    auto &physicsWorld = s_Scene->physicsWorld;
    physicsWorld.Step(dt);
// Fixed update Time
//    if(accumulateTime <= MS_PER_UPDATE)
//    {
//        accumulateTime += dt;
//    }
//    else
//    {
//        while(accumulateTime >= MS_PER_UPDATE)
//        {
//            accumulateTime -= MS_PER_UPDATE;
//            physicsWorld.Step(MS_PER_UPDATE);
//        }
//    }
}

void PhysicsSystem::InitPhysicsObject(entt::registry &registry, Scene::SceneState state)
{
    // Physics Record
    auto &physicsWorld = s_Scene->physicsWorld;
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;
    // State Record check it exists
    auto &statePhysicsObject = s_Scene->StatePhysicsObj;
    auto &stateBoxColliderObject = s_Scene->StateBoxColliderObj;
    auto &stateJointObject = s_Scene->StateJointObj;

    // RigidBody2D Component
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
            statePhysicsObject.push_back(std::make_pair(UUID, true));
            // Add Force on Point
            auto attx = transform.translate.x + rigidbody2D.attachPoint.x;
            auto atty = transform.translate.y + rigidbody2D.attachPoint.y;
            physicsObj->AddForce(rigidbody2D.force, Vec2(attx, atty));
            physicsWorld.Add(physicsObj);
        }
    }

    // BoxCollider2DComponent Component
    auto group2 = registry.view<TransformComponent, BoxCollider2DComponent>();
    for(auto entity : group2)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &boxc = registry.get<BoxCollider2DComponent>(entity);
        // BoxCollider Component
        if(!mapBoxColliderObj.count(UUID))
        {
            auto box = MakeRef<Box>(boxc.x, boxc.y, boxc.w, boxc.h);
            mapBoxColliderObj[UUID] = box;
            stateBoxColliderObject.push_back(std::make_pair(UUID, true));
        }
    }

    // Joint2DComponent Component
    auto group3 = registry.view<TransformComponent, Joint2DComponent>();
    for(auto entity : group3)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &jit = registry.get<Joint2DComponent>(entity);
        auto &mapJointObj = s_Scene->mapJointObj;

        // BoxCollider Component
        if(!mapJointObj.count(UUID))
        {
            auto jit = MakeRef<Joint>();
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &jitComponent = registry.get<Joint2DComponent>(entity);
            jit->SetAnchor(jitComponent.anchor);
            jit->SetBodies(mapPhysicsObj[jitComponent.rigidBody1], mapPhysicsObj[jitComponent.rigidBody2]);
            mapJointObj[UUID] = jit;
            stateJointObject.push_back(std::make_pair(UUID, true));
            physicsWorld.AddJoints(jit);
        }
    }
}

void PhysicsSystem::UpdateNewPhysicsObject(entt::registry& registry, Scene::SceneState state)
{
    auto &physicsWorld = s_Scene->physicsWorld;

    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

    auto &statePhysicsObject = s_Scene->StatePhysicsObj;
    auto &stateBoxColliderObject = s_Scene->StateBoxColliderObj;
    auto &stateJointObject = s_Scene->StateJointObj;

    // Initial all state, it will be false
    for(auto &statephy : statePhysicsObject)
    {
        statephy.second = false;
    }
    for(auto &stateboxc : stateBoxColliderObject)
    {
        stateboxc.second = false;
    }
    for(auto &statejit : stateJointObject)
    {
        statejit.second = false;
    }

    // Update RigidBody2D Component data to engine
    auto view = registry.view<TransformComponent, RigidBody2DComponent>();
    for (auto entity : view)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);
        // Update state Record, exist true
        for (auto &state : statePhysicsObject)
        {
            if(state.first == UUID)
            {
                state.second = true;
            }
        }

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

    // Update BoxCollider2D component data to engine
    auto view2 = registry.view<TransformComponent, BoxCollider2DComponent>();
    for (auto entity : view2) {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &boxcollider2D = registry.get<BoxCollider2DComponent>(entity);
        // Update state Record
        for (auto &state : stateBoxColliderObject)
        {
            if(state.first == UUID)
            {
                state.second = true;
            }
        }
        if (mapBoxColliderObj.count(UUID))
        {
            auto &boxc = mapBoxColliderObj[UUID];
            boxc->x = boxcollider2D.x;
            boxc->y = boxcollider2D.y;
            boxc->w = boxcollider2D.w;
            boxc->h = boxcollider2D.h;
        }

    }

    // Update Joint2D component data to engine
    auto view3 = registry.view<TransformComponent, Joint2DComponent>();
    for (auto entity : view3)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        // update joint state
        for (auto &state : stateJointObject)
        {
            if(state.first == UUID)
                state.second = true;
        }
    }

    // Remove PhysicsWorld Object
    // RigidBody2D Component
    for(auto &state: statePhysicsObject)
    {
        if (state.second == false) {
            mapPhysicsObj.erase(state.first);
        }
    }
    // BoxCollider2DComponent Component
    for(auto &state: stateBoxColliderObject)
    {
        if(state.second == false)
        {
            mapBoxColliderObj.erase(state.first);
        }
    }
    // Joint2DComponent Component
    for(auto &state: stateJointObject)
    {
        if(state.second == false)
        {
            mapJointObj.erase(state.first);
        }
    }

    // Remove State Record if object is deleted in scene
    auto pred = [](auto x)
    {
        return x.second == false;
    };
    auto it = std::remove_if(statePhysicsObject.begin(), statePhysicsObject.end(), pred);
    statePhysicsObject.erase(it, statePhysicsObject.end());
    auto it2 = std::remove_if(stateBoxColliderObject.begin(), stateBoxColliderObject.end(), pred);
    stateBoxColliderObject.erase(it2, stateBoxColliderObject.end());
    auto it3 = std::remove_if(stateJointObject.begin(), stateJointObject.end(), pred);
    stateJointObject.erase(it3, stateJointObject.end());

    // Add New Object in RunTime
    // RigidBody2D Component
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
            statePhysicsObject.push_back(std::make_pair(UUID, true));
            // Add Force on Point
            auto attx = transform.translate.x + rigidbody2D.attachPoint.x;
            auto atty = transform.translate.y + rigidbody2D.attachPoint.y;
            physicsObj->AddForce(rigidbody2D.force, Vec2(attx, atty));
            physicsWorld.Add(physicsObj);
        }
    }

    // BoxCollider2DComponent Component
    auto group2 = registry.view<TransformComponent, BoxCollider2DComponent>();
    for(auto entity : group2)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &boxc = registry.get<BoxCollider2DComponent>(entity);
        // BoxCollider Component
        if(!mapBoxColliderObj.count(UUID))
        {
            auto box = MakeRef<Box>(boxc.x, boxc.y, boxc.w, boxc.h);
            mapBoxColliderObj[UUID] = box;
            stateBoxColliderObject.push_back(std::make_pair(UUID, true));
        }
    }

    // Joint2DComponent Component
    auto group3 = registry.view<TransformComponent, Joint2DComponent>();
    for(auto entity : group3)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &jit = registry.get<Joint2DComponent>(entity);
        auto &mapJointObj = s_Scene->mapJointObj;

        // BoxCollider Component
        if(!mapJointObj.count(UUID))
        {
            auto jit = MakeRef<Joint>();
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &jitComponent = registry.get<Joint2DComponent>(entity);
            jit->SetAnchor(jitComponent.anchor);
            jit->SetBodies(mapPhysicsObj[jitComponent.rigidBody1], mapPhysicsObj[jitComponent.rigidBody2]);
            mapJointObj[UUID] = jit;
            stateJointObject.push_back(std::make_pair(UUID, true));
            physicsWorld.AddJoints(jit);
        }
    }
}


}

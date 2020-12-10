#include <Rish/Scene/Entity.h>
#include <Rish/Events/Event.h>
#include <Rish/Physics/PhysicsSystem.h>

#include <Rish/Collider/Box.h>
#include <Rish/Collider/Circle.h>
#include <Rish/Collider/Polygon.h>

namespace rl {

static float accumulateTime = 0.0f;
static float MS_PER_UPDATE = 1.0f / 60.0f;
static bool checkState = false;

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
    auto &mapColliderObj = s_Scene->mapColliderObj;
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
        auto group = registry.view<TransformComponent, RigidBody2DComponent, Collider2DComponent>();
        for (auto entity : group) {
            auto &UUID = registry.get<TagComponent>(entity).id;
            auto &transform = registry.get<TransformComponent>(entity);
            auto &rigidbody2D = registry.get<RigidBody2DComponent>(entity);
            auto &collider2D = registry.get<Collider2DComponent>(entity);

            if (mapPhysicsObj.count(UUID) && mapColliderObj.count(UUID)) {
                auto &phy = mapPhysicsObj[UUID];
                auto &collider = mapColliderObj[UUID];

                float offset_x = phy->position.x - (transform.translate.x + collider->position.x);
                float offset_y = phy->position.y - (transform.translate.y + collider->position.y);

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

            if (!mapColliderObj.count(UUID) && mapPhysicsObj.count(UUID)) {
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

void PhysicsSystem::OnEditorUpdate(std::set<Entity> &selectedEntites)
{
    // Clear not selected rigidBody restrict state
    auto group2 =  s_Scene->m_registry.view<TransformComponent, RigidBody2DComponent>();
    for (auto entity : group2)
    {
        auto &rigid =  s_Scene->m_registry.get<RigidBody2DComponent>(entity);
        rigid.engineRestrictGravity = false;
        // Editor enforce restrict Gravity
        if(rigid.RestrictGravity)
        {
            rigid.engineRestrictGravity = true;
        }
    }
    // selected rigidBody will be restrict state
    if(ImGui::GetIO().MouseDown[0])
    {
        for(auto ent : selectedEntites)
        {
            if(ent.hasComponent<RigidBody2DComponent>())
            {
                ent.getComponent<RigidBody2DComponent>().engineRestrictGravity = true;
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
    auto &mapColliderObj = s_Scene->mapColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;
    // clear physics object
    physicsWorld.Clear();
    mapPhysicsObj.clear();
    mapColliderObj.clear();
    mapJointObj.clear();
}

void PhysicsSystem::OnImGuiRender()
{
    // Physics World
    auto &physicsWorld = s_Scene->physicsWorld;
    auto &mapJointObj = s_Scene->mapJointObj;
    auto &mapColliderObj = s_Scene->mapColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;
    // State Record check it exists
    auto &statePhysicsObject = s_Scene->StatePhysicsObj;
    auto &stateColliderObject = s_Scene->StateColliderObj;
    auto &stateJointObject = s_Scene->StateJointObj;

    auto &registry = s_Scene->m_registry;

    // Restrict Gravity
    ImGui::Begin("Physics Debug");

    for(auto mp : mapPhysicsObj)
    {
        ImGui::Text("%d",mp.second->shape->type);
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
    auto &mapColliderObj = s_Scene->mapColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;
    // State Record check it exists
    auto &statePhysicsObject = s_Scene->StatePhysicsObj;
    auto &stateColliderObject = s_Scene->StateColliderObj;
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
            // Default for box Collider
            auto physicsObj = MakeRef<RigidBody2D>(RigidBody2D::Type::Box);
            physicsObj->setBox(Vec2(transform.translate.x, transform.translate.y), Vec2(transform.scale.x, transform.scale.y), transform.rotate);
            mapPhysicsObj[UUID] = physicsObj;
            statePhysicsObject.push_back(std::make_pair(UUID, true));

            // Add Force on Point
            auto attx = transform.translate.x + rigidbody2D.attachPoint.x;
            auto atty = transform.translate.y + rigidbody2D.attachPoint.y;
            physicsObj->AddForce(rigidbody2D.force, Vec2(attx, atty));

            // Default no collider
            physicsObj->isCollider = false;
            physicsWorld.Add(physicsObj);
        }
    }

    // Collider2DComponent Component
    auto group2 = registry.view<TransformComponent, Collider2DComponent>();
    for(auto entity : group2)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &collider2D = registry.get<Collider2DComponent>(entity);
        // Collider Component
        if(!mapColliderObj.count(UUID))
        {
            switch(collider2D.type)
            {
                case Collider2DComponent::Type::Box:
                {
                    auto box = MakeRef<Box>(collider2D.x, collider2D.y, collider2D.w, collider2D.h, glm::radians(transform.rotate));
                    box->type = Shape::Type::Box;
                    mapColliderObj[UUID] = box;
                    break;
                }
                case Collider2DComponent::Type::Circle:
                {
                    auto circle = MakeRef<Circle>(collider2D.x, collider2D.y, collider2D.radius, glm::radians(transform.rotate));
                    circle->type = Shape::Type::Circle;
                    mapColliderObj[UUID] = circle;
                    break;
                }
                case Collider2DComponent::Type::Polygon:
                {
                    auto polygon = MakeRef<Polygon>(collider2D.pt, Vec2(collider2D.x, collider2D.y), collider2D.pointSize, glm::radians(transform.rotate));
                    polygon->type = Shape::Type::Polygon;
                    mapColliderObj[UUID] = polygon;
                    break;
                }
            }
            stateColliderObject.push_back(std::make_pair(UUID, true));
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
    auto &mapColliderObj = s_Scene->mapColliderObj;
    auto &mapPhysicsObj = s_Scene->mapPhysicsObj;

    auto &statePhysicsObject = s_Scene->StatePhysicsObj;
    auto &stateColliderObject = s_Scene->StateColliderObj;
    auto &stateJointObject = s_Scene->StateJointObj;

    // Initial all state, it will be false, suppose all object is not exist
    for(auto &statephy : statePhysicsObject)
    {
        statephy.second = false;
    }
    for(auto &statec : stateColliderObject)
    {
        statec.second = false;
    }
    for(auto &statejit : stateJointObject)
    {
        statejit.second = false;
    }

    // Update BoxCollider2D component data to engine
    auto view2 = registry.view<TransformComponent, Collider2DComponent>();
    for (auto entity : view2)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &collider2D = registry.get<Collider2DComponent>(entity);

        // Update state Record
        for (auto &state : stateColliderObject)
        {
            if(state.first == UUID)
            {
                state.second = true;
            }
        }

        if (mapColliderObj.count(UUID))
        {
            auto &collider = mapColliderObj[UUID];
            switch(collider->type)
            {
                case Shape::Type::Box:
                {
                    collider->position.x = collider2D.x;
                    collider->position.y = collider2D.y;
                    collider->w = collider2D.w;
                    collider->h = collider2D.h;
                    break;
                }
                case Shape::Type::Circle:
                {
                    collider->position.x = collider2D.x;
                    collider->position.y = collider2D.y;
                    collider->radius = collider2D.radius;
                    break;
                }
                case Shape::Type::Polygon:
                {
                    collider->position.x = collider2D.x;
                    collider->position.y = collider2D.y;
                    for(int i = 0; i < collider2D.pointSize; i++)
                    {
                        collider->pt[i] = collider2D.pt[i];
                    }
                    break;
                }
            }
        }
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
        if (mapColliderObj.count(UUID) && mapPhysicsObj.count(UUID))
        {
            auto &phy = mapPhysicsObj[UUID];
            auto &collider = mapColliderObj[UUID];

            // Transform parameter
            phy->position.x = transform.translate.x;
            phy->position.y = transform.translate.y;
            phy->angle = glm::radians(transform.rotate);

            // Physics parameter
            phy->mass = rigidbody2D.mass;
            phy->friction = rigidbody2D.friction;
            phy->velocity = rigidbody2D.velocity;
            phy->force = rigidbody2D.force;
            phy->attachPoint = rigidbody2D.attachPoint;
            phy->keepingForce = rigidbody2D.keepingForce;
            phy->RestrictGravity = rigidbody2D.engineRestrictGravity;
            phy->isCollider = true;

            // Collider Case
            switch(collider->type)
            {
                case Shape::Type::Box:
                {
                    phy->setBox(Vec2(transform.translate.x + collider->position.x, transform.translate.y + collider->position.y), Vec2(collider->w, collider->h), glm::radians(transform.rotate) );
                    phy->shape = phy->box;
                    // https://stackoverflow.com/questions/12321949/explicitly-deleting-a-shared-ptr
                    delete phy->circle.get();
                    delete phy->polygon.get();
                    phy->shape->type = Shape::Type::Box;
                    phy->RigidShapeType = RigidBody2D::Type::Box;
                    break;
                }
                case Shape::Type::Circle:
                {
                    phy->setCircle(Vec2(transform.translate.x + collider->position.x, transform.translate.y + collider->position.y), collider->radius, glm::radians(transform.rotate));
                    phy->shape = phy->circle;
                    delete phy->box.get();
                    delete phy->polygon.get();
                    phy->shape->type = Shape::Type::Circle;
                    phy->RigidShapeType = RigidBody2D::Type::Circle;
                    break;
                }
                case Shape::Type::Polygon:
                {
                    phy->setPolygon(collider->pt, Vec2(transform.translate.x + collider->position.x, transform.translate.y + collider->position.y), collider->pointSize, glm::radians(transform.rotate));
                    phy->shape = phy->polygon;
                    delete phy->box.get();
                    delete phy->circle.get();
                    phy->shape->type = Shape::Type::Polygon;
                    phy->RigidShapeType = RigidBody2D::Type::Polygon;
                    break;
                }
            }
        }
        // Only RigidBody2D
        else if (mapPhysicsObj.count(UUID))
        {
            auto &phy = mapPhysicsObj[UUID];
            phy->position.x = transform.translate.x;
            phy->position.y = transform.translate.y;
            phy->wh.x = transform.scale.x;
            phy->wh.y = transform.scale.y;
            // no Collider, default Box
            phy->setBox(Vec2(transform.translate.x, transform.translate.y), Vec2(transform.scale.x, transform.scale.y), glm::radians(transform.rotate));

            phy->angle = glm::radians(transform.rotate);
            phy->mass = rigidbody2D.mass;
            phy->friction = rigidbody2D.friction;
            phy->velocity = rigidbody2D.velocity;
            phy->force = rigidbody2D.force;
            phy->attachPoint = rigidbody2D.attachPoint;
            phy->keepingForce = rigidbody2D.keepingForce;
            phy->RestrictGravity = rigidbody2D.engineRestrictGravity;
            phy->isCollider = false;
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
    for(auto &state: stateColliderObject)
    {
        if(state.second == false)
        {
            mapColliderObj.erase(state.first);
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
    auto it2 = std::remove_if(stateColliderObject.begin(), stateColliderObject.end(), pred);
    stateColliderObject.erase(it2, stateColliderObject.end());
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
            auto physicsObj = MakeRef<RigidBody2D>(RigidBody2D::Type::Box);
            physicsObj->setBox(Vec2(transform.translate.x, transform.translate.y), Vec2(transform.scale.x, transform.scale.y), rigidbody2D.mass);
            physicsObj->shape = physicsObj->box;
            mapPhysicsObj[UUID] = physicsObj;
            statePhysicsObject.push_back(std::make_pair(UUID, true));
            // Add Force on Point
            auto attx = transform.translate.x + rigidbody2D.attachPoint.x;
            auto atty = transform.translate.y + rigidbody2D.attachPoint.y;
            physicsObj->AddForce(rigidbody2D.force, Vec2(attx, atty));
            physicsWorld.Add(physicsObj);
        }
    }

    // Collider2DComponent
    auto group2 = registry.view<TransformComponent, Collider2DComponent>();
    for(auto entity : group2)
    {
        auto &UUID = registry.get<TagComponent>(entity).id;
        auto &transform = registry.get<TransformComponent>(entity);
        auto &collider2D = registry.get<Collider2DComponent>(entity);
        // Collider Component
        if(!mapColliderObj.count(UUID))
        {
            switch(collider2D.type)
            {
                case Collider2DComponent::Type::Box:
                {
                    auto box = MakeRef<Box>(collider2D.x, collider2D.y, collider2D.w, collider2D.h, glm::radians(transform.rotate));
                    box->type = Shape::Type::Box;
                    mapColliderObj[UUID] = box;
                    break;
                }
                case Collider2DComponent::Type::Circle:
                {
                    auto circle = MakeRef<Circle>(collider2D.x, collider2D.y, collider2D.radius, glm::radians(transform.rotate));
                    circle->type = Shape::Type::Circle;
                    mapColliderObj[UUID] = circle;
                    break;
                }
                case Collider2DComponent::Type::Polygon:
                {
                    auto polygon = MakeRef<Polygon>(collider2D.pt, Vec2(collider2D.x, collider2D.y), collider2D.pointSize, glm::radians(transform.rotate));
                    polygon->type = Shape::Type::Polygon;
                    mapColliderObj[UUID] = polygon;
                    break;
                }
            }
            stateColliderObject.push_back(std::make_pair(UUID, true));
        }
    }

    // Joint2DComponent
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

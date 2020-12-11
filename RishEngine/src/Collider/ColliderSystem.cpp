#include <Rish/Collider/ColliderSystem.h>

#include <Rish/Scene/Component.h>
#include <Rish/Scene/Scene.h>
//
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Collider/Box.h>
//
#include <Rish/ImGui/ImGui.h>

namespace rl {

Ref<Scene> ColliderSystem::s_Scene;

void ColliderSystem::RegisterScene(const Ref <Scene> &scene)
{
    s_Scene = scene;
}

void ColliderSystem::OnUpdate(float dt)
{
    auto state = s_Scene->getSceneState();
    if(state == Scene::SceneState::Pause)
        return;

    // TODO: other shape collider
    auto view = s_Scene->m_registry.view<TransformComponent, BoxCollider2DComponent>();
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;

    // TODO: update QuadTree
    // Clear All State
    for (auto &box : view)
    {
        auto &boxCollider = s_Scene->m_registry.get<BoxCollider2DComponent>(box);
        if(!boxCollider.trigger) continue;
        boxCollider.isCollision = false;
        boxCollider.whoCollide.clear();
    }

    auto view2 = s_Scene->m_registry.view<TransformComponent, BoxCollider2DComponent>();
    for (auto &boxA : view2)
    {
        auto &boxCollider = s_Scene->m_registry.get<BoxCollider2DComponent>(boxA);
        if(!boxCollider.trigger) continue;

        for(auto &boxB : view2)
        {
            auto &UUID_A = s_Scene->m_registry.get<TagComponent>(boxA).id;
            auto &boxColliderA = s_Scene->m_registry.get<BoxCollider2DComponent>(boxA);
            auto &transA = s_Scene->m_registry.get<TransformComponent>(boxA);

            auto &UUID_B = s_Scene->m_registry.get<TagComponent>(boxB).id;
            auto &boxColliderB = s_Scene->m_registry.get<BoxCollider2DComponent>(boxB);
            auto &transB = s_Scene->m_registry.get<TransformComponent>(boxB);

            // Update Check
            auto engineBoxA = MakeRef<Box>(transA.translate.x + boxColliderA.x, transA.translate.y + boxColliderA.y, boxColliderA.w, boxColliderA.h);
            engineBoxA->rotation = transA.rotate;
            auto engineBoxB = MakeRef<Box>(transB.translate.x + boxColliderB.x, transB.translate.y + boxColliderB.y, boxColliderB.w, boxColliderB.h);
            engineBoxB->rotation = transB.rotate;

            // Existed Check
            auto &boxAWhoCollide = boxColliderA.whoCollide;
            auto &boxBWhoCollide = boxColliderB.whoCollide;

            // UUID Same, Pass This Time
            if(UUID_A == UUID_B)
            {
                continue;
            }
            if(std::find(boxAWhoCollide.begin(), boxAWhoCollide.end(), UUID_B) != boxAWhoCollide.end())
            {
                continue;
            }
            if(std::find(boxBWhoCollide.begin(), boxBWhoCollide.end(), UUID_A) != boxBWhoCollide.end())
            {
                continue;
            }

            // Check who Collide
            if (engineBoxA->isCollide(engineBoxB))
            {
                boxColliderA.whoCollide.push_back(UUID_B);
                boxColliderA.isCollision = true;
                boxColliderB.whoCollide.push_back(UUID_A);
                boxColliderB.isCollision = true;
            }
        }
    }
}

}
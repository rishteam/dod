#include <Rish/Collider/ColliderSystem.h>


namespace rl {

Ref<Scene> ColliderSystem::s_Scene;

void ColliderSystem::onInit(const Ref <Scene> &scene)
{
    s_Scene = scene;
}

void ColliderSystem::onUpdate(entt::registry& registry, float dt, Scene::SceneState state)
{
    // TODO: other collider
    auto view = registry.view<TransformComponent, BoxCollider2DComponent>();
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    // TODO: update QuadTree

    // Clear All State
    for (auto &box : view)
    {
        auto &boxCollider = registry.get<BoxCollider2DComponent>(box);
        boxCollider.isCollision = false;
    }

    auto view2 = registry.view<TransformComponent, BoxCollider2DComponent>();
    for (auto &boxA : view2)
    {
        for(auto &boxB : view2)
        {
            auto &UUID_A = registry.get<TagComponent>(boxA).id;
            auto &boxColliderA = registry.get<BoxCollider2DComponent>(boxA);
            auto &transA = registry.get<TransformComponent>(boxA);

            auto &UUID_B = registry.get<TagComponent>(boxB).id;
            auto &boxColliderB = registry.get<BoxCollider2DComponent>(boxB);
            auto &transB = registry.get<TransformComponent>(boxB);

            // UUID Same, Pass This Time
            if(UUID_A == UUID_B)
            {
                break;
            }

            // Update Check
            auto engineBoxA = MakeRef<Box>(transA.translate.x + boxColliderA.x, transA.translate.y + boxColliderA.y, boxColliderA.w, boxColliderA.h);
            engineBoxA->rotation = transA.rotate;
            auto engineBoxB = MakeRef<Box>(transB.translate.x + boxColliderB.x, transB.translate.y + boxColliderB.y, boxColliderB.w, boxColliderB.h);
            engineBoxB->rotation = transB.rotate;


            if (engineBoxA->isCollide(engineBoxB))
            {
                boxColliderA.whoCollide = UUID_B;
                boxColliderA.isCollision = true;
                boxColliderB.whoCollide = UUID_A;
                boxColliderB.isCollision = true;
            }
        }
    }

}

};
#include <Rish/Collider/ColliderSystem.h>


namespace rl {

Ref<Scene> ColliderSystem::s_Scene;

void ColliderSystem::onUpdate(entt::registry& registry, float dt, Scene::SceneState state)
{
    // TODO: other collider
    // auto view = registry.view<TransformComponent, BoxCollider2DComponent>();
    auto &mapBoxColliderObj = s_Scene->mapBoxColliderObj;
    // TODO: update QuadTree

    // Clear All State
    auto view = registry.view<TransformComponent, BoxCollider2DComponent>();
    for (auto box : view)
    {
        auto &boxCollider = registry.get<BoxCollider2DComponent>(box);
        boxCollider.isCollision = false;
    }

    for (auto boxA : view)
    {
        auto &UUID_A = registry.get<TagComponent>(boxA).id;
        auto &boxColliderA = registry.get<BoxCollider2DComponent>(boxA);

        for(auto boxB : view)
        {
            auto &UUID_B = registry.get<TagComponent>(boxB).id;
            auto &boxColliderB = registry.get<BoxCollider2DComponent>(boxB);

            // UUID Same, Pass This Time
            if(UUID_A == UUID_B)
            {
                break;
            }
            // Update Check
            auto engineBoxA = mapBoxColliderObj[UUID_A];
            auto engineBoxB = mapBoxColliderObj[UUID_B];
            if (engineBoxA->isCollide(engineBoxB))
            {
                boxColliderA.whoCollide = UUID_B;
                boxColliderA.isCollision = true;
            }
        }
    }

}

};
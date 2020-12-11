#include <Rish/Scene/Entity.h>
#include <Rish/Collider/ColliderSystem.h>
#include <Rish/Collider/Shape.h>
#include <Rish/Collider/Box.h>
#include <Rish/Collider/Circle.h>
#include <Rish/Collider/Polygon.h>


namespace rl {

Ref<Scene> ColliderSystem::s_Scene;

void ColliderSystem::RegisterScene(const Ref <Scene> &scene)
{
    s_Scene = scene;
}

void ColliderSystem::OnEditorUpdate(std::set<Entity> &selectedEntites)
{
    // Selected Collider
    if(ImGui::GetIO().MouseDown[0])
    {
        Ref <Shape> A;
        Ref <Shape> B;

        for(auto ent : selectedEntites)
        {
            if(ent.hasComponent<Collider2DComponent>())
            {
                auto &UUID_A = ent.getComponent<TagComponent>().id;
                auto &transA = ent.getComponent<TransformComponent>();
                auto &colliderA = ent.getComponent<Collider2DComponent>();
                colliderA.whoCollide.clear();
                colliderA.isCollision = false;

                // Update Check
                switch (colliderA.type) {
                    case Collider2DComponent::Type::Box: {
                        A = MakeRef<Box>(transA.translate.x + colliderA.x, transA.translate.y + colliderA.y, colliderA.w,  colliderA.h,
                                         glm::radians(transA.rotate));
                        A->type = Shape::Type::Box;
                        break;
                    }
                    case Collider2DComponent::Type::Circle: {
                        A = MakeRef<Circle>(transA.translate.x + colliderA.x, transA.translate.y + colliderA.y, colliderA.radius,
                                            glm::radians(transA.rotate));
                        A->type = Shape::Type::Circle;
                        break;
                    }
                    case Collider2DComponent::Type::Polygon: {
                        A = MakeRef<Polygon>(colliderA.pt, Vec2(transA.translate.x + colliderA.x, transA.translate.y + colliderA.y),
                                             colliderA.pointSize, glm::radians(transA.rotate));
                        A->type = Shape::Type::Polygon;
                        break;
                    }
                }

                // All Collider
                auto view = s_Scene->m_registry.view<TransformComponent, Collider2DComponent>();
                for(auto entity : view)
                {
                    auto &UUID_B = s_Scene->m_registry.get<TagComponent>(entity).id;
                    auto &colliderB = s_Scene->m_registry.get<Collider2DComponent>(entity);
                    auto &transB = s_Scene->m_registry.get<TransformComponent>(entity);

                    switch (colliderB.type) {
                        case Collider2DComponent::Type::Box: {
                            B = MakeRef<Box>(transB.translate.x + colliderB.x, transB.translate.y + colliderB.y, colliderB.w, colliderB.h,
                                             glm::radians(transB.rotate));
                            B->type = Shape::Type::Box;
                            break;
                        }
                        case Collider2DComponent::Type::Circle: {
                            B = MakeRef<Circle>(transB.translate.x + colliderB.x, transB.translate.y + colliderB.y, colliderB.radius,
                                                glm::radians(transB.rotate));
                            B->type = Shape::Type::Circle;
                            break;
                        }
                        case Collider2DComponent::Type::Polygon: {
                            B = MakeRef<Polygon>(colliderB.pt, Vec2(transB.translate.x + colliderB.x, transB.translate.y + colliderB.y),
                                                 colliderB.pointSize, glm::radians(transB.rotate));
                            B->type = Shape::Type::Polygon;
                            break;
                        }
                    }

                    if(UUID_A == UUID_B)
                    {
                        continue;
                    }
                    if (A->isCollide(B)) {
                        colliderA.whoCollide.push_back(UUID_B);
                        colliderA.isCollision = true;
                    }

                }
            }
        }
    }
}

}
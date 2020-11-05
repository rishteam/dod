#include <Rish/Script/ObjectController.h>

namespace rl {


void ObjectController::onCreate()
{

}

void ObjectController::onDestroy()
{

}

void ObjectController::onUpdate(Time dt)
{
    auto view = GetScene().m_registry.view<TransformComponent, BoxCollider2DComponent, SpriteRenderComponent>();

    for (auto ent : view) {
        auto entity = GetEntityByHandle(ent);

        auto &trans = entity.getComponent<TransformComponent>();
        auto &boxc = entity.getComponent<BoxCollider2DComponent>();
        auto &render = entity.getComponent<SpriteRenderComponent>();
        auto &rigid = entity.getComponent<RigidBody2DComponent>();


        switch (objectState) {
            case ObjectState::Left:
                rigid.velocity.x = -1.0f;
                break;
            case ObjectState::Right:
                rigid.velocity.x = 1.0f;
                break;
        }

        for (auto uuid : boxc.whoCollide) {
            auto collider = GetScene().getEntityByUUID(uuid);

            auto colliderTrans = collider.getComponent<TransformComponent>();
            // Judge Collide
            if (std::max(trans.translate.x - trans.scale.x / 2, colliderTrans.translate.x - colliderTrans.translate.x / 2) <=
                std::min(trans.translate.x + trans.scale.x / 2, colliderTrans.translate.x + colliderTrans.translate.x / 2)) {
                if (objectState == ObjectState::Left) {
                    objectState = ObjectState::Right;
                } else if (objectState == ObjectState::Right) {
                    objectState = ObjectState::Left;
                }
            }
        }
    }
}

void ObjectController::onImGuiRender()
{

}


}
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
    auto view = GetScene().m_registry.view<TransformComponent, Collider2DComponent, SpriteRenderComponent, RigidBody2DComponent, NativeScriptComponent>();

    for (auto ent : view) {
        auto entity = GetEntityByHandle(ent);
        // Check NativeScript Type
        if(entity.getComponent<NativeScriptComponent>().scriptName == "rl::ObjectController")
        {
            auto &trans = entity.getComponent<TransformComponent>();
            auto &boxc = entity.getComponent<Collider2DComponent>();
            auto &render = entity.getComponent<SpriteRenderComponent>();
            auto &rigid = entity.getComponent<RigidBody2DComponent>();

            switch (objectState)
            {
                case ObjectState::Left:
                    rigid.velocity.x = -1.0f;
                    break;
                case ObjectState::Right:
                    rigid.velocity.x = 1.0f;
                    break;
            }

            // if state Change, rigid value must will be calculated by physics
            if(stateChange == true)
            {
                stateChange = false;
                return;
            }

            for (auto uuid : boxc.whoCollide) {
                auto collider = GetScene().getEntityByUUID(uuid);

                auto colliderTrans = collider.getComponent<TransformComponent>();
                // Judge Collide
                if (std::max(trans.translate.x - trans.scale.x / 2, colliderTrans.translate.x - colliderTrans.translate.x / 2) <=
                    std::min(trans.translate.x + trans.scale.x / 2, colliderTrans.translate.x + colliderTrans.translate.x / 2))
                {
                    if (objectState == ObjectState::Left)
                    {
                        objectState = ObjectState::Right;
                    }
                    else if (objectState == ObjectState::Right)
                    {
                        objectState = ObjectState::Left;
                    }
                    stateChange = true;
                }
            }
        }
    }
}

void ObjectController::onImGuiRender()
{
    const char* items[] = { "Empty", "Mushroom", "Star"};
    static int item_current = 0;
    ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
    objectType = static_cast<ObjectType>(item_current);
}

void ObjectController::killObject()
{

}


}
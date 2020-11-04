#include <Rish/Script/EventBoxController.h>

namespace rl {

void EventBoxController::onCreate()
{

}

void EventBoxController::onDestroy()
{

}

void EventBoxController::spawnObject(EventContent eventContent)
{

    switch (eventContent) {
        case EventContent::Empty:
        {
            break;
        }
        case EventContent::Mushroom:
        {
            auto trans = GetEntity().getComponent<TransformComponent>();
            Entity ent = CreateEntity(fmt::format("New Object"), glm::vec3(trans.translate.x, trans.translate.y + trans.scale.y + 0.5, trans.translate.z));

            ent.addComponent<SpriteRenderComponent>("assets\\texture\\mario\\items.png");
            ent.addComponent<RigidBody2DComponent>(10.f);
            ent.addComponent<BoxCollider2DComponent>(0.f, 0.f, trans.scale.x, trans.scale.y);
            // TODO: NativeScript bind failed
//            ent.addComponent<NativeScriptComponent>().bind<MonsterController>(ent);

            auto &render = ent.getComponent<SpriteRenderComponent>();
            auto &newObjectTrans = ent.getComponent<TransformComponent>();
            auto &rigid = ent.getComponent<RigidBody2DComponent>();
            rigid.RestrictRotation = true;
            newObjectTrans.scale.x = 0.5f;
            newObjectTrans.scale.y = 0.5f;

            // Change
            SubTexture2DSetting setting;
            setting.type = SubTexture2DSetting::SubTextureCoordinate;
            setting.leftUpper = glm::vec2(182, 83);
            setting.size = glm::vec2(21, 21);

            //
            render.loadSubTexture(setting);
            break;
        }
        case EventContent::Star:
        {

            break;
        }
    }
}

void EventBoxController::onUpdate(Time dt)
{
    auto view = GetScene().m_registry.view<TransformComponent, BoxCollider2DComponent, SpriteRenderComponent>();

    for (auto ent : view)
    {
        auto entity = GetEntityByHandle(ent);
        auto &trans = entity.getComponent<TransformComponent>();
        auto &boxc = entity.getComponent<BoxCollider2DComponent>();
        auto &render = entity.getComponent<SpriteRenderComponent>();
        if (boxc.isCollision)
        {
            // whoCollide List
            for (auto uuid : boxc.whoCollide)
            {
                // Check NativeScript Exists
                auto collideEntity = GetScene().getEntityByUUID(uuid);
                if (collideEntity.hasComponent<NativeScriptComponent>() &&
                    entity.hasComponent<NativeScriptComponent>())
                {
                    // Check Script Type
                    if (collideEntity.getComponent<NativeScriptComponent>().scriptName == "rl::PlayerController"
                        && entity.getComponent<NativeScriptComponent>().scriptName == "rl::EventBoxController")
                    {
                        auto playerCoordinate = collideEntity.getComponent<TransformComponent>().translate;
                        auto playerWh = collideEntity.getComponent<TransformComponent>().scale;
                        auto boxCoordinate = trans.translate;
                        auto boxWh = trans.scale;

                        // Judge collision
                        if (boxCoordinate.y - boxCoordinate.y / 2 - playerCoordinate.y + playerCoordinate.y / 2 > 0 &&
                            std::min(playerCoordinate.x + playerWh.x / 2, boxCoordinate.x + boxWh.x / 2) -
                            std::max(playerCoordinate.x - playerWh.x / 2, boxCoordinate.x - boxWh.x / 2) >= boxWh.x / 2)
                        {
                            // Change block State
                            render.loadTexture("assets\\texture\\mario\\blocks.png");
                            //
                            SubTexture2DSetting setting;
                            setting.type = SubTexture2DSetting::SubTextureCoordinate;
                            setting.leftUpper = glm::vec2(145, 143);
                            setting.size = glm::vec2(14, 14);
                            //
                            render.loadSubTexture(setting);

                            if (contentCount >= 1)
                            {
                                spawnObject(eventContent);
                                contentCount -= 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

void EventBoxController::onImGuiRender()
{
    const char* items[] = { "Empty", "Mushroom", "Star"};
    static int item_current = 0;
    ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
    eventContent = static_cast<EventContent>(item_current);

    ImGui::InputInt("Spawn Object Counter", &contentCount);
}

}
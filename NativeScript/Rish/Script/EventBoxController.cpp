#include <Rish/Script/EventBoxController.h>

namespace rl {

void EventBoxController::onCreate()
{

}

void EventBoxController::onDestroy()
{

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
                // check NativeScript Exists
                auto collideEntity = GetScene().getEntityByUUID(uuid);
                if (collideEntity.hasComponent<NativeScriptComponent>() &&
                    entity.hasComponent<NativeScriptComponent>())
                {
                    // check Script Type
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
                            render.loadTexture("assets\\texture\\mario\\blocks.png");
                            //
                            SubTexture2DSetting setting;
                            setting.type = SubTexture2DSetting::SubTextureCoordinate;
                            setting.leftUpper = glm::vec2(145, 143);
                            setting.size = glm::vec2(14, 14);
                            //
                            render.loadSubTexture(setting);
                        }
                    }
                }
            }
        }
    }
}
void EventBoxController::onImGuiRender()
{

}

}
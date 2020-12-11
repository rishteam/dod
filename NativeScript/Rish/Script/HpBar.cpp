#include <Rish/Script/HpBar.h>

namespace rl {

void HpBar::onCreate()
{

}

void HpBar::onDestroy()
{

}

void HpBar::onUpdate(Time dt)
{
    auto &trans = GetComponent<TransformComponent>();

    auto player = GetScene().getEntityByUUID("0accdf49-1dd2-48b2-af88-33e23d565a01");
    auto &playerLight = player.getComponent<LightComponent>();
    trans.scale.x = playerLight.strength;

    auto view = GetScene().m_registry.view<TransformComponent, CameraComponent>();

    for(auto ent : view)
    {
        auto entity = GetEntityByHandle(ent);
        auto &camera = entity.getComponent<CameraComponent>();

        if(camera.primary)
        {

        }
    }
}

void HpBar::onImGuiRender()
{

}

}


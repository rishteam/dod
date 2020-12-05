#include <Rish/Script/Player.h>

namespace rl {

void Player::onCreate()
{
}

void Player::onDestroy()
{

}

void Player::onUpdate(Time dt)
{
    auto &trans = GetComponent<TransformComponent>();
    auto &rigid = GetComponent<RigidBody2DComponent>();
    auto &render = GetComponent<SpriteRenderComponent>();

    Vec2 &velocity = rigid.velocity;
    if (Input::IsKeyPressed(Keyboard::Left))
    {
        if(velocity.x >= -walkSpeedLimit)
            velocity.x -= walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
        if(velocity.x <= walkSpeedLimit)
            velocity.x += walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Down))
    {
        if(velocity.y >= -walkSpeedLimit)
            velocity.y -= walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Up))
    {
        if(velocity.y <= walkSpeedLimit)
            velocity.y += walkAccel * dt.asSeconds();
    }

    // TODO: Trigger with Object (ex: mushroom, star, monster...)
//    detectObject();

    // Camera follows player
    auto view = GetScene().m_registry.view<TransformComponent, CameraComponent>();
    for(auto ent : view)
    {
        auto entity = GetEntityByHandle(ent);
        auto &camera = entity.getComponent<CameraComponent>();
        auto &transform = entity.getComponent<TransformComponent>();
        if(camera.primary)
        {
            transform.translate.x = trans.translate.x;
            transform.translate.y = trans.translate.y;
        }
    }
}

void Player::onImGuiRender()
{
    ImGui::DragFloat("Walk Accelerate", &walkAccel, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Walk Speed Limit", &walkSpeedLimit, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Jump Speed", &jumpSpeed, 10.0f, 0.0f, 100.0f);
}

}

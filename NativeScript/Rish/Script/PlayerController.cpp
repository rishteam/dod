#include <Rish/Script/PlayerController.h>

namespace rl {

void PlayerController::onCreate()
{

}

void PlayerController::onDestroy()
{

}

void PlayerController::onUpdate(Time dt)
{
    auto &trans = GetComponent<TransformComponent>();
    auto &rigid = GetComponent<RigidBody2DComponent>();
    auto &boxc = GetComponent<BoxCollider2DComponent>();
    rigid.attachPoint = Vec2(0.0f, 0.0f);
    // TODO: HOT Fixed
    trans.rotate = 0.0f;

    if (Input::IsKeyPressed(Keyboard::Up))
    {
        rigid.velocity.y += y_force_speed * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Down))
    {
        rigid.velocity.y -= y_force_speed * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Left))
    {
        rigid.velocity.x -= x_force_speed * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
        rigid.velocity.x += x_force_speed * dt.asSeconds();
    }
}

void PlayerController::onImGuiRender()
{
    ImGui::DragFloat("x_force_speed", &x_force_speed, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("y_force_speed", &y_force_speed, 10.0f, 0.0f, 100.0f);
}

}